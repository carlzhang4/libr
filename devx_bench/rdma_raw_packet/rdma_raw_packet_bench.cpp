#include <gflags/gflags.h>
#include <hdr/hdr_histogram.h>

#include "libr.hpp"
using namespace std;
std::mutex IO_LOCK;


int BUF_SIZE;
int NUM_THREADS;
int NUMA_NODE;
string DEVICE_NAME;
int FLOW_UDP_DST_PORT;
int CORE_OFFSET;
int PKT_SIZE;
bool IS_SERVER;

std::atomic<bool> stop_flag = false;

// client: sudo ./rdma_raw_packet_bench -deviceName mlx5_2
// server: sudo ./rdma_raw_packet_bench -deviceName mlx5_2 -server

// bf1 enp3s0f0s0
unsigned char CLIENT_MAC_ADDR[6] = { 0x02,0x15,0x9e,0x7c,0x4d,0xad };
// bf2 enp3s0f0s0
unsigned char SERVER_MAC_ADDR[6] = { 0x02,0xbd,0xe9,0x97,0x48,0xd3 };

void ctrl_c_handler(int) { stop_flag = true; }

DEFINE_int32(threads, 1, "num_threads");
DEFINE_int32(numaNode, 0, "numaNode");
DEFINE_string(deviceName, "mlx5_0", "deviceName");
DEFINE_int32(flow_udp_dst_port, 6666, "bind flow to udp dst port");
DEFINE_int32(coreOffset, 0, "coreOffset");
DEFINE_int32(pktSize, 1024, "packet size");
DEFINE_bool(server, false, "server mode");

static uint32_t NB_RXD = 1024;
static uint32_t NB_TXD = 1024;
static uint32_t PKT_BUF_SIZE = 12288;
static uint32_t HANDLE_BATCH = 8;
static uint32_t SEND_OUTSTANDING = 64;


void sub_task_server(int thread_index, QpHandler *handler) {
    wait_scheduling(thread_index, IO_LOCK);

    struct ibv_wc *wc_recv = NULL;
    ALLOCATE(wc_recv, struct ibv_wc, CTX_POLL_BATCH);

    size_t local_mr_addr = reinterpret_cast<size_t>(handler->buf);
    size_t local_rkey = handler->mr->lkey;
    OffsetHandler recv(NB_RXD, PKT_BUF_SIZE, NB_TXD * PKT_BUF_SIZE);
    OffsetHandler recv_comp(NB_RXD, PKT_BUF_SIZE, NB_TXD * PKT_BUF_SIZE);

    size_t rx_depth = handler->rx_depth;

    for (size_t i = 0;i < rx_depth; i++) {
        handler->recv_sge_list[0].addr = recv.offset() + local_mr_addr;
        handler->recv_sge_list[0].length = PKT_BUF_SIZE;
        handler->recv_sge_list[0].lkey = local_rkey;
        handler->recv_wr->num_sge = 1;
        handler->recv_wr->sg_list = handler->recv_sge_list;
        handler->recv_wr->wr_id = recv.offset() + local_mr_addr;
        handler->recv_wr->next = nullptr;

        assert(ibv_post_recv(handler->qp, handler->recv_wr, &handler->recv_bar_wr) == 0);
        recv.step();
    }

    for (int i = 0;i < handler->num_wrs;i++) {
        handler->recv_sge_list[i].lkey = local_rkey;
        handler->recv_wr[i].sg_list = handler->recv_sge_list + i;
        handler->recv_wr[i].num_sge = 1;
        if (i != 0) {
            handler->recv_wr[i - 1].next = handler->recv_wr + i;
        }
        handler->recv_wr[i].next = nullptr;
    }

    TimeUtil global_timer;
    while (!stop_flag) {
        if ((recv.index() - recv_comp.index()) < (rx_depth - HANDLE_BATCH)) {
            for (size_t i = 0;i < HANDLE_BATCH;i++) {
                handler->recv_sge_list[i].addr = recv.offset() + local_mr_addr;
                handler->recv_sge_list[i].length = PKT_BUF_SIZE;
                handler->recv_wr[i].wr_id = recv.offset() + local_mr_addr;
                if (i == HANDLE_BATCH - 1) {
                    handler->recv_wr[i].next = nullptr;
                }
                recv.step();
            }
            assert(ibv_post_recv(handler->qp, handler->recv_wr, &handler->recv_bar_wr) == 0);
        }
        int ne_recv = ibv_poll_cq(handler->recv_cq, CTX_POLL_BATCH, wc_recv);
        if (ne_recv != 0) {
            global_timer.start_once();
        }
        for (int i = 0;i < ne_recv;i++) {
            assert(wc_recv[i].status == IBV_WC_SUCCESS);
            assert(wc_recv[i].byte_len == static_cast<uint32_t>(PKT_SIZE));
            recv_comp.step();
        }
    }
    global_timer.end();

    double duration = global_timer.get_seconds();
    double recv_speed = 8.0 * recv_comp.index() * PKT_SIZE / 1000 / 1000 / 1000 / duration;
    std::lock_guard<std::mutex> lock(IO_LOCK);
    LOG_I("thread [%d], duration [%f]s, recv speed [%f] Gbps", thread_index, duration, recv_speed);

    free(wc_recv);
}

void sub_task_client(int thread_index, QpHandler *handler) {
    wait_scheduling(thread_index, IO_LOCK);

    struct ibv_wc *wc_send = NULL;
    ALLOCATE(wc_send, struct ibv_wc, CTX_POLL_BATCH);

    size_t local_mr_addr = reinterpret_cast<size_t>(handler->buf);
    size_t local_rkey = handler->mr->lkey;

    OffsetHandler send(NB_TXD, PKT_BUF_SIZE, 0);
    OffsetHandler send_comp(NB_TXD, PKT_BUF_SIZE, 0);

    size_t tx_depth = handler->tx_depth;

    for (int i = 0;i < handler->num_wrs;i++) {
        handler->send_sge_list[i].lkey = local_rkey;
        handler->send_wr[i].sg_list = handler->send_sge_list + i;
        handler->send_wr[i].num_sge = 1;
        if (i != 0) {
            handler->send_wr[i - 1].next = handler->send_wr + i;
        }
        handler->send_wr[i].next = nullptr;
        handler->send_wr[i].send_flags = IBV_SEND_IP_CSUM;
        handler->send_wr[i].opcode = IBV_WR_SEND;
    }

    for (size_t i = 0;i < tx_depth;i++) {
        udp_packet *now_pkt = reinterpret_cast<udp_packet *>(local_mr_addr + PKT_BUF_SIZE * i);
        now_pkt->eth_hdr.ether_type = htons(0x0800);
        for (size_t j = 0;j < 6;j++) {
            now_pkt->eth_hdr.dst_addr.addr_bytes[j] = SERVER_MAC_ADDR[j];
            now_pkt->eth_hdr.src_addr.addr_bytes[j] = CLIENT_MAC_ADDR[j];
        }

        now_pkt->ip_hdr.version_ihl = 0x45;
        now_pkt->ip_hdr.type_of_service = 0;
        now_pkt->ip_hdr.total_length = htons(PKT_SIZE - sizeof(ether_hdr));
        now_pkt->ip_hdr.packet_id = htons(0);
        now_pkt->ip_hdr.fragment_offset = htons(0);
        now_pkt->ip_hdr.time_to_live = 64;
        now_pkt->ip_hdr.next_proto_id = 17;
        now_pkt->ip_hdr.src_addr = 0;
        now_pkt->ip_hdr.dst_addr = 0;

        now_pkt->udp_hdr.dgram_len = htons(PKT_SIZE - sizeof(ether_hdr) - sizeof(ipv4_hdr));
        now_pkt->udp_hdr.src_port = FLOW_UDP_DST_PORT;
        now_pkt->udp_hdr.dst_port = htons(FLOW_UDP_DST_PORT + thread_index);
    }

    TimeUtil global_timer;
    global_timer.start_once();
    while (!stop_flag) {
        if ((send.index() - send_comp.index()) < (SEND_OUTSTANDING - HANDLE_BATCH)) {
            for (size_t i = 0;i < HANDLE_BATCH;i++) {
                handler->send_sge_list[i].addr = send.offset() + local_mr_addr;
                handler->send_sge_list[i].length = PKT_SIZE;
                handler->send_wr[i].wr_id = send.index();
                if (i == HANDLE_BATCH - 1) {
                    handler->send_wr[i].next = nullptr;
                    handler->send_wr[i].send_flags |= IBV_SEND_SIGNALED;
                }
                send.step();
            }
            assert(ibv_post_send(handler->qp, handler->send_wr, &handler->send_bar_wr) == 0);
        }

        int ne_send = ibv_poll_cq(handler->send_cq, CTX_POLL_BATCH, wc_send);
        for (int i = 0;i < ne_send;i++) {
            assert(wc_send[i].status == IBV_WC_SUCCESS);
            send_comp.step(HANDLE_BATCH);
        }
    }
    global_timer.end();
    double duration = global_timer.get_seconds();
    double send_speed = 8.0 * send_comp.index() * PKT_SIZE / 1000 / 1000 / 1000 / duration;
    std::lock_guard<std::mutex> lock(IO_LOCK);
    LOG_I("thread [%d], duration [%f]s, send speed [%f] Gbps", thread_index, duration, send_speed);

    free(wc_send);
}

void benchmark(NetParam &net_param) {
    int num_cpus = thread::hardware_concurrency();
    LOG_I("%-20s : %d", "HardwareConcurrency", num_cpus);
    assert(NUM_THREADS <= num_cpus);

    BUF_SIZE = (NB_TXD + NB_RXD) * PKT_BUF_SIZE;

    void **bufs = new void *[NUM_THREADS];
    QpHandler **qp_handlers = new QpHandler * [NUM_THREADS]();
    for (int i = 0;i < NUM_THREADS;i++) {
        bufs[i] = malloc_2m_numa(BUF_SIZE, net_param.numa_node);
        for (int j = 0;j < BUF_SIZE / static_cast<int>(sizeof(int));j++) {
            (reinterpret_cast<int **>(bufs))[i][j] = j;
        }
    }

    for (int i = 0;i < NUM_THREADS;i++) {
        qp_handlers[i] = create_qp_raw_packet(net_param, bufs[i], BUF_SIZE, NB_TXD, NB_RXD, i);
    }

    ibv_flow **main_flow = new ibv_flow * [NUM_THREADS];

    size_t flow_attr_total_size = sizeof(ibv_flow_attr) + sizeof(ibv_flow_spec_eth) + sizeof(ibv_flow_spec_tcp_udp);

    void *header_buff = malloc(flow_attr_total_size);
    memset(header_buff, 0, flow_attr_total_size);
    ibv_flow_attr *flow_attr = reinterpret_cast<ibv_flow_attr *>(header_buff);
    ibv_flow_spec_eth *flow_spec_eth = reinterpret_cast<ibv_flow_spec_eth *>(flow_attr + 1);
    ibv_flow_spec_tcp_udp *flow_spec_udp = reinterpret_cast<ibv_flow_spec_tcp_udp *>(flow_spec_eth + 1);
    flow_attr->size = flow_attr_total_size;
    flow_attr->priority = 0;
    flow_attr->num_of_specs = 2;
    flow_attr->port = 1;
    flow_attr->flags = 0;
    flow_attr->type = IBV_FLOW_ATTR_NORMAL;

    flow_spec_eth->type = IBV_FLOW_SPEC_ETH;
    flow_spec_eth->size = sizeof(ibv_flow_spec_eth);
    flow_spec_eth->val.ether_type = htons(0x0800);
    flow_spec_eth->mask.ether_type = 0xffff;
    if (IS_SERVER) {
        memcpy(flow_spec_eth->val.dst_mac, SERVER_MAC_ADDR, 6);
    } else {
        memcpy(flow_spec_eth->val.dst_mac, CLIENT_MAC_ADDR, 6);
    }
    memset(flow_spec_eth->mask.dst_mac, 0xFF, 6);

    for (int i = 0;i < NUM_THREADS;i++) {
        flow_spec_udp->type = IBV_FLOW_SPEC_UDP;
        flow_spec_udp->size = sizeof(ibv_flow_spec_tcp_udp);
        flow_spec_udp->val.dst_port = htons(FLOW_UDP_DST_PORT + i);
        flow_spec_udp->mask.dst_port = 0xFFFF;
        // flow_spec_udp->val.src_port = htons(SMARTNS_UDP_MAGIC_PORT + i);
        // flow_spec_udp->mask.src_port = 0xFFFF;
        ibv_flow *flow = ibv_create_flow(qp_handlers[i]->qp, flow_attr);
        assert(flow);
        main_flow[i] = flow;
    }

    free(header_buff);

    vector<thread> threads(NUM_THREADS);
    for (int i = 0;i < NUM_THREADS;i++) {
        int now_index = get_cpu_index_with_numa(i + CORE_OFFSET, net_param.numa_node);
        if (IS_SERVER) {
            threads[i] = thread(sub_task_server, now_index, qp_handlers[i]);
        } else {
            threads[i] = thread(sub_task_client, now_index, qp_handlers[i]);
        }
        set_cpu_with_numa(threads[i], i + CORE_OFFSET, net_param.numa_node);
    }

    for (int i = 0;i < NUM_THREADS;i++) {
        threads[i].join();
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        assert(ibv_destroy_flow(main_flow[i]) == 0);
    }
    delete[]main_flow;

    for (int i = 0;i < NUM_THREADS;i++) {
        free(qp_handlers[i]->send_sge_list);
        free(qp_handlers[i]->recv_sge_list);
        free(qp_handlers[i]->send_wr);
        free(qp_handlers[i]->recv_wr);

        ibv_destroy_qp(qp_handlers[i]->qp);
        ibv_dereg_mr(qp_handlers[i]->mr);
        ibv_destroy_cq(qp_handlers[i]->send_cq);
        ibv_destroy_cq(qp_handlers[i]->recv_cq);
        ibv_dealloc_pd(qp_handlers[i]->pd);

        ibv_close_device(net_param.contexts[i]);
        free(qp_handlers[i]);
    }

    delete[]bufs;
    delete[]qp_handlers;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, ctrl_c_handler);
    signal(SIGTERM, ctrl_c_handler);

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    NUM_THREADS = FLAGS_threads;
    NUMA_NODE = FLAGS_numaNode;
    DEVICE_NAME = FLAGS_deviceName;
    FLOW_UDP_DST_PORT = FLAGS_flow_udp_dst_port;
    CORE_OFFSET = FLAGS_coreOffset;
    PKT_SIZE = FLAGS_pktSize;
    IS_SERVER = FLAGS_server;
    if (PKT_SIZE < 64 || static_cast<uint32_t>(PKT_SIZE) > PKT_BUF_SIZE) {
        LOG_E("pkt size must be in [64, %u]", PKT_BUF_SIZE);
        return -1;
    }

    if ((NUM_THREADS & (NUM_THREADS - 1)) != 0) {
        LOG_E("NUM_THREADS must be power of 2");
        return -1;
    }

    NetParam net_param;
    net_param.numNodes = 2;
    net_param.device_name = DEVICE_NAME;
    net_param.numa_node = NUMA_NODE;
    net_param.batch_size = HANDLE_BATCH;
    net_param.sge_per_wr = 1;
    net_param.nodeId = 0;
    net_param.sock_port = FLOW_UDP_DST_PORT;

    net_param.ib_port = 1;
    net_param.page_size = sysconf(_SC_PAGESIZE);
    net_param.cacheline_size = get_cache_line_size();

    roce_init(net_param, NUM_THREADS);
    benchmark(net_param);
}