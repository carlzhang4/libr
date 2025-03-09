#include <iostream>
#include <thread>
#include <mutex>
#include <cstdio>
#include <stdio.h>
#include <time.h>
#include <atomic>
#include <gflags/gflags.h>
#include <queue> 
#include <fstream>
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

static uint8_t RSS_KEY[40] = { 0x6d, 0x5a, 0x56, 0xda, 0x25, 0x5b, 0x0e, 0xc2,
                              0x41, 0x67, 0x25, 0x3d, 0x43, 0xa3, 0x8f, 0xb0,
                              0xd0, 0xca, 0x2b, 0xcb, 0xae, 0x7b, 0x30, 0xb4,
                              0x77, 0xcb, 0x2d, 0xa3, 0x80, 0x30, 0xf2, 0x0c,
                              0x6a, 0x42, 0xb7, 0x3b, 0xbe, 0xac, 0x01, 0xfa };

// static uint8_t SERVER_MAC_ADDR[6] = { 0x02, 0xc3, 0x7c, 0x0f,0x71 ,0xb9 };
// static uint8_t CLIENT_MAC_ADDR[6] = { 0xa0, 0x88, 0xc2, 0x31, 0xf7, 0xde };
// static uint8_t SERVER_MAC_ADDR[6] = { 0xa0, 0x88, 0xc2, 0x32, 0x04, 0x30 };

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
static uint32_t PKT_BUF_SIZE = 2048;
static uint32_t HANDLE_BATCH = 1;
static uint32_t SEND_OUTSTANDING = 64;

class WqHandler {
public:
    ibv_context *context;
    ibv_pd *pd;
    ibv_mr *mr;

    ibv_cq *recv_cq;
    ibv_wq *recv_wq;
    ibv_sge *recv_sge_list;
    ibv_recv_wr *recv_wr;
    ibv_recv_wr *recv_bar_wr;

    ibv_cq *send_cq;
    ibv_qp *send_qp;
    ibv_sge *send_sge_list;
    ibv_send_wr *send_wr;
    ibv_send_wr *send_bar_wr;

    int max_inline_size;
    int num_wrs;
    int num_sges_per_wr;
    int num_sges;
    int tx_depth;
    int rx_depth;
    size_t buf;
};

WqHandler *create_wq(NetParam &net_param, ibv_pd *pd, void *buf, size_t size) {
    assert(net_param.num_contexts > 0);
    WqHandler *wq_handler;
    ALLOCATE(wq_handler, WqHandler, 1);

    int rx_depth = NB_RXD;
    int tx_depth = NB_TXD;
    uint32_t max_inline_size = 0;

    int num_wrs = net_param.batch_size != 0 ? net_param.batch_size : 1;
    int num_sges = num_wrs;
    struct ibv_sge *recv_sge_list, *send_sge_list;
    struct ibv_recv_wr *recv_wr;
    struct ibv_send_wr *send_wr;
    struct ibv_recv_wr *recv_bad_wr;
    struct ibv_send_wr *send_bad_wr;

    ALLOCATE(recv_sge_list, struct ibv_sge, num_sges);
    ALLOCATE(recv_wr, struct ibv_recv_wr, num_wrs);
    ALLOCATE(recv_bad_wr, struct ibv_recv_wr, 1);

    ALLOCATE(send_sge_list, struct ibv_sge, num_sges);
    ALLOCATE(send_wr, struct ibv_send_wr, num_wrs);
    ALLOCATE(send_bad_wr, struct ibv_send_wr, 1);

    //check valid mem
    assert(size > static_cast<size_t>(net_param.page_size));
    assert((reinterpret_cast<size_t>(buf)) % net_param.page_size == 0);

    struct ibv_mr *mr;
    struct ibv_cq *recv_cq, *send_cq;
    struct ibv_wq *recv_wq;
    struct ibv_qp *send_qp;
    assert(mr = ibv_reg_mr(pd, buf, size, IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE | IBV_ACCESS_REMOTE_READ));
    assert(recv_cq = ibv_create_cq(net_param.contexts[0], rx_depth, NULL, NULL, 0));

    // create wq
    ibv_wq_init_attr wq_init_attr;
    memset(&wq_init_attr, 0, sizeof(wq_init_attr));
    wq_init_attr.wq_context = nullptr;
    wq_init_attr.wq_type = IBV_WQT_RQ;
    wq_init_attr.max_wr = rx_depth;
    wq_init_attr.max_sge = 1;
    wq_init_attr.pd = pd;
    wq_init_attr.cq = recv_cq;
    wq_init_attr.create_flags = 0;

    assert(recv_wq = ibv_create_wq(net_param.contexts[0], &wq_init_attr));

    struct ibv_wq_attr wq_attr;
    memset(&wq_attr, 0, sizeof(struct ibv_wq_attr));
    wq_attr.wq_state = IBV_WQS_RDY;
    wq_attr.attr_mask = IBV_WQ_ATTR_STATE;
    assert(ibv_modify_wq(recv_wq, &wq_attr) == 0);

    // create send qp
    assert(send_cq = ibv_create_cq(net_param.contexts[0], tx_depth, NULL, NULL, 0));
    struct ibv_qp_init_attr tx_qp_init_attr;
    memset(&tx_qp_init_attr, 0, sizeof(tx_qp_init_attr));
    tx_qp_init_attr.send_cq = send_cq;
    tx_qp_init_attr.recv_cq = recv_cq;
    tx_qp_init_attr.cap.max_send_wr = tx_depth;
    tx_qp_init_attr.cap.max_send_sge = 1;
    tx_qp_init_attr.qp_type = IBV_QPT_RAW_PACKET;
    assert(send_qp = ibv_create_qp(pd, &tx_qp_init_attr));

    struct ibv_qp_attr tx_qp_attr;
    memset(&tx_qp_attr, 0, sizeof(tx_qp_attr));
    tx_qp_attr.qp_state = IBV_QPS_INIT;
    tx_qp_attr.port_num = 1;
    assert(ibv_modify_qp(send_qp, &tx_qp_attr, IBV_QP_STATE | IBV_QP_PORT) == 0);

    memset(&tx_qp_attr, 0, sizeof(tx_qp_attr));
    tx_qp_attr.qp_state = IBV_QPS_RTR;
    assert(ibv_modify_qp(send_qp, &tx_qp_attr, IBV_QP_STATE) == 0);

    memset(&tx_qp_attr, 0, sizeof(tx_qp_attr));
    tx_qp_attr.qp_state = IBV_QPS_RTS;
    assert(ibv_modify_qp(send_qp, &tx_qp_attr, IBV_QP_STATE) == 0);

    wq_handler->context = net_param.contexts[0];
    wq_handler->pd = pd;
    wq_handler->mr = mr;

    wq_handler->recv_cq = recv_cq;
    wq_handler->recv_wq = recv_wq;
    wq_handler->recv_sge_list = recv_sge_list;
    wq_handler->recv_wr = recv_wr;
    wq_handler->recv_bar_wr = recv_bad_wr;

    wq_handler->send_cq = send_cq;
    wq_handler->send_qp = send_qp;
    wq_handler->send_sge_list = send_sge_list;
    wq_handler->send_wr = send_wr;
    wq_handler->send_bar_wr = send_bad_wr;

    wq_handler->max_inline_size = max_inline_size;
    wq_handler->num_wrs = num_wrs;
    wq_handler->num_sges_per_wr = 1;
    wq_handler->num_sges = num_sges;
    wq_handler->rx_depth = rx_depth;
    wq_handler->tx_depth = tx_depth;
    wq_handler->buf = reinterpret_cast<size_t>(buf);
    return wq_handler;
}

QpHandler *create_qp_raw_packet(NetParam &net_param, ibv_pd *pd, ibv_rwq_ind_table *rwq_ind_table) {
    QpHandler *qp_handler;
    ALLOCATE(qp_handler, QpHandler, 1);

    struct ibv_qp *qp;

    struct ibv_qp_init_attr_ex qp_init_attr_ex;
    memset(&qp_init_attr_ex, 0, sizeof(qp_init_attr_ex));

    qp_init_attr_ex.qp_context = nullptr;
    qp_init_attr_ex.srq = nullptr;
    qp_init_attr_ex.cap.max_inline_data = 0;
    // qp_init_attr_ex.cap.max_send_wr = rx_depth;
    // qp_init_attr_ex.cap.max_send_sge = 1;
    // qp_init_attr_ex.cap.max_recv_wr = rx_depth;
    // qp_init_attr_ex.cap.max_recv_sge = 1;
    qp_init_attr_ex.qp_type = IBV_QPT_RAW_PACKET;

    qp_init_attr_ex.pd = pd;
    qp_init_attr_ex.create_flags = 0; //ibv_qp_create_flags 
    qp_init_attr_ex.rwq_ind_tbl = rwq_ind_table;
    qp_init_attr_ex.rx_hash_conf.rx_hash_function = IBV_RX_HASH_FUNC_TOEPLITZ;
    qp_init_attr_ex.rx_hash_conf.rx_hash_key_len = 40;
    qp_init_attr_ex.rx_hash_conf.rx_hash_key = RSS_KEY;
    qp_init_attr_ex.rx_hash_conf.rx_hash_fields_mask = IBV_RX_HASH_SRC_IPV4 | IBV_RX_HASH_DST_IPV4 |
        IBV_RX_HASH_SRC_PORT_UDP | IBV_RX_HASH_DST_PORT_UDP;

    qp_init_attr_ex.comp_mask = IBV_QP_INIT_ATTR_PD | IBV_QP_INIT_ATTR_IND_TABLE | IBV_QP_INIT_ATTR_RX_HASH;

    assert(qp = ibv_create_qp_ex(net_param.contexts[0], &qp_init_attr_ex));

    qp_handler->pd = pd;
    qp_handler->qp = qp;

    return qp_handler;
}

void sub_recv_server(int thread_index, WqHandler *handler) {
    wait_scheduling(thread_index, IO_LOCK);
    TimeUtil global_timer;
    struct ibv_wc *wc_recv = NULL, *wc_send = NULL;
    ALLOCATE(wc_recv, struct ibv_wc, CTX_POLL_BATCH);
    ALLOCATE(wc_send, struct ibv_wc, CTX_POLL_BATCH);

    size_t local_mr_addr = reinterpret_cast<size_t>(handler->buf);
    size_t local_rkey = handler->mr->lkey;

    OffsetHandler send(NB_TXD, PKT_BUF_SIZE, 0);
    OffsetHandler send_comp(NB_TXD, PKT_BUF_SIZE, 0);
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

        assert(ibv_post_wq_recv(handler->recv_wq, handler->recv_wr, &handler->recv_bar_wr) == 0);
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

    while (!stop_flag) {
        int ne_recv = ibv_poll_cq(handler->recv_cq, CTX_POLL_BATCH, wc_recv);
        if (ne_recv != 0) {
            global_timer.start_once();
        }
        for (int i = 0;i < ne_recv;i++) {
            assert(wc_recv[i].status == IBV_WC_SUCCESS);
            // assert(wc_recv[i].byte_len == static_cast<uint32_t>(PKT_BUF_SIZE));

            uint8_t *eth_header = reinterpret_cast<uint8_t *>(wc_recv[i].wr_id);
            size_t src_mac = *(size_t *)eth_header;
            size_t dst_mac = *(size_t *)(eth_header + 6);

            *(size_t *)(eth_header) = dst_mac;
            *(size_t *)(eth_header + 6) = (src_mac & 0x0000FFFFFFFFFFFF) | (0x0008ll << 48);
            uint32_t now_recv_index = recv_comp.index() % HANDLE_BATCH;

            handler->send_sge_list[now_recv_index].addr = wc_recv[i].wr_id;
            handler->send_sge_list[now_recv_index].length = wc_recv[i].byte_len;
            handler->send_wr[now_recv_index].wr_id = send.index();

            if (now_recv_index == HANDLE_BATCH - 1) {
                //这里不用把next恢复，因为如果是最后一个，那之后也不用收包了
                handler->send_wr[now_recv_index].next = nullptr;
                handler->send_wr[now_recv_index].send_flags = IBV_SEND_SIGNALED;
                assert(ibv_post_send(handler->send_qp, handler->send_wr, &handler->send_bar_wr) == 0);
            }
            recv_comp.step();
            send.step();
        }

        int ne_send = ibv_poll_cq(handler->send_cq, CTX_POLL_BATCH, wc_send);
        for (int i = 0;i < ne_send;i++) {
            assert(wc_send[i].status == IBV_WC_SUCCESS);
            int now_send_begin = send_comp.index();
            int now_send_end = wc_send[i].wr_id;
            for (int j = now_send_begin;j <= now_send_end;j++) {
                int index = j - now_send_begin;
                handler->recv_sge_list[index].addr = recv.offset() + local_mr_addr;
                handler->recv_sge_list[index].length = PKT_BUF_SIZE;
                handler->recv_wr[index].wr_id = recv.offset() + local_mr_addr;
                recv.step();
            }
            handler->recv_wr[now_send_end - now_send_begin].next = nullptr;
            assert(ibv_post_wq_recv(handler->recv_wq, handler->recv_wr, &handler->recv_bar_wr) == 0);

            send_comp.step(now_send_end - now_send_begin + 1);
        }
    }
    if (recv_comp.index()) {
        global_timer.end();
        double duration = global_timer.get_seconds();
        double speed_pps = recv_comp.index() / duration / 1e6;

        std::lock_guard<std::mutex> lock(IO_LOCK);
        LOG_I("thread [%d], duration [%f]s, PPS [%f] Mops", thread_index, duration, speed_pps);
    }
    free(wc_recv);
    free(wc_send);
}

void sub_send_server(int thread_index, WqHandler *handler) {
    wait_scheduling(thread_index, IO_LOCK);
    TimeUtil global_timer;
    struct ibv_wc *wc_recv = NULL, *wc_send = NULL;
    ALLOCATE(wc_recv, struct ibv_wc, CTX_POLL_BATCH);
    ALLOCATE(wc_send, struct ibv_wc, CTX_POLL_BATCH);

    size_t local_mr_addr = reinterpret_cast<size_t>(handler->buf);
    size_t local_rkey = handler->mr->lkey;

    OffsetHandler send(NB_TXD, PKT_BUF_SIZE, 0);
    OffsetHandler send_comp(NB_TXD, PKT_BUF_SIZE, 0);
    OffsetHandler recv(NB_RXD, PKT_BUF_SIZE, NB_TXD * PKT_BUF_SIZE);
    OffsetHandler recv_comp(NB_RXD, PKT_BUF_SIZE, NB_TXD * PKT_BUF_SIZE);

    size_t rx_depth = handler->rx_depth;
    size_t tx_depth = handler->tx_depth;

    // initial recv wqe
    for (size_t i = 0;i < rx_depth; i++) {
        handler->recv_sge_list[0].addr = recv.offset() + local_mr_addr;
        handler->recv_sge_list[0].length = PKT_BUF_SIZE;
        handler->recv_sge_list[0].lkey = local_rkey;
        handler->recv_wr->num_sge = 1;
        handler->recv_wr->sg_list = handler->recv_sge_list;
        handler->recv_wr->wr_id = recv.offset() + local_mr_addr;
        handler->recv_wr->next = nullptr;

        assert(ibv_post_wq_recv(handler->recv_wq, handler->recv_wr, &handler->recv_bar_wr) == 0);
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

    // initial send wqe
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
        now_pkt->ip_hdr.src_addr = htonl(i);
        now_pkt->ip_hdr.dst_addr = htonl(i);

        now_pkt->udp_hdr.dgram_len = htons(PKT_SIZE - sizeof(ether_hdr) - sizeof(ipv4_hdr));
        now_pkt->udp_hdr.src_port = htons(i);
        now_pkt->udp_hdr.dst_port = htons(FLOW_UDP_DST_PORT);
    }

    global_timer.start_once();
    while (!stop_flag) {
        if ((send.index() - recv_comp.index()) < (SEND_OUTSTANDING - HANDLE_BATCH)) {
            for (size_t i = 0;i < HANDLE_BATCH;i++) {
                handler->send_sge_list[i].addr = send.offset() + local_mr_addr;
                handler->send_sge_list[i].length = PKT_SIZE;
                handler->send_wr[i].wr_id = send.index();
                if (i == HANDLE_BATCH - 1) {
                    handler->send_wr[i].next = nullptr;
                    handler->send_wr[i].send_flags = IBV_SEND_SIGNALED;
                }
                send.step();
            }
            assert(ibv_post_send(handler->send_qp, handler->send_wr, &handler->send_bar_wr) == 0);
        }

        int ne_send = ibv_poll_cq(handler->send_cq, CTX_POLL_BATCH, wc_send);
        for (int i = 0;i < ne_send;i++) {
            assert(wc_send[i].status == IBV_WC_SUCCESS);
            send_comp.step(HANDLE_BATCH);
        }

        int ne_recv = ibv_poll_cq(handler->recv_cq, CTX_POLL_BATCH, wc_recv);
        for (int i = 0;i < ne_recv;i++) {
            assert(wc_recv[i].status == IBV_WC_SUCCESS);

            uint32_t now_recv_index = recv_comp.index() % HANDLE_BATCH;

            handler->recv_sge_list[now_recv_index].addr = recv.offset() + local_mr_addr;
            handler->recv_sge_list[now_recv_index].length = PKT_BUF_SIZE;
            handler->recv_wr[now_recv_index].wr_id = recv.offset() + local_mr_addr;
            recv_comp.step();
            recv.step();
            if (now_recv_index == HANDLE_BATCH - 1) {
                handler->recv_wr[now_recv_index].next = nullptr;
                assert(ibv_post_wq_recv(handler->recv_wq, handler->recv_wr, &handler->recv_bar_wr) == 0);
            }
        }
    }
    global_timer.end();
    double duration = global_timer.get_seconds();
    double send_speed_pps = send_comp.index() / duration / 1e6;
    double recv_speed_pps = recv_comp.index() / duration / 1e6;
    std::lock_guard<std::mutex> lock(IO_LOCK);
    LOG_I("thread [%d], duration [%f]s, send/recv PPS [%f,%f] Mops", thread_index, duration, send_speed_pps, recv_speed_pps);

    free(wc_recv);
    free(wc_send);

}

void benchmark(NetParam &net_param) {
    int num_cpus = thread::hardware_concurrency();
    LOG_I("%-20s : %d", "HardwareConcurrency", num_cpus);
    assert(NUM_THREADS <= num_cpus);

    BUF_SIZE = (NB_TXD + NB_RXD) * PKT_BUF_SIZE;

    WqHandler **wq_handlers = new WqHandler * [NUM_THREADS]();

    ibv_pd *main_pd = ibv_alloc_pd(net_param.contexts[0]);
    assert(main_pd);

    void **bufs = new void *[NUM_THREADS];
    for (int i = 0;i < NUM_THREADS;i++) {
        bufs[i] = malloc_2m_numa(BUF_SIZE, net_param.numa_node);
        for (int j = 0;j < BUF_SIZE / static_cast<int>(sizeof(int));j++) {
            (reinterpret_cast<int **>(bufs))[i][j] = 0;
        }
    }

    for (int i = 0;i < NUM_THREADS;i++) {
        wq_handlers[i] = create_wq(net_param, main_pd, bufs[i], BUF_SIZE);
    }

    ibv_wq **wqs = new ibv_wq * [NUM_THREADS];
    for (int i = 0;i < NUM_THREADS;i++) {
        wqs[i] = wq_handlers[i]->recv_wq;
    }
    ibv_rwq_ind_table_init_attr rwq_ind_table_init_attr;
    rwq_ind_table_init_attr.log_ind_tbl_size = std::log2(NUM_THREADS);
    rwq_ind_table_init_attr.ind_tbl = wqs;
    rwq_ind_table_init_attr.comp_mask = 0;

    ibv_rwq_ind_table *rwq_ind_table;
    assert(rwq_ind_table = ibv_create_rwq_ind_table(net_param.contexts[0], &rwq_ind_table_init_attr));

    delete[]wqs;

    QpHandler *main_qp = create_qp_raw_packet(net_param, main_pd, rwq_ind_table);

    size_t flow_attr_total_size = sizeof(ibv_flow_attr) + sizeof(ibv_flow_spec_eth) + sizeof(ibv_flow_spec_tcp_udp);

    void *header_buff = malloc(flow_attr_total_size);
    memset(header_buff, 0, flow_attr_total_size);
    ibv_flow_attr *flow_attr = reinterpret_cast<ibv_flow_attr *>(header_buff);
    ibv_flow_spec_eth *flow_spec_eth = reinterpret_cast<ibv_flow_spec_eth *>(flow_attr + 1);
    ibv_flow_spec_tcp_udp *flow_spec_udp = reinterpret_cast<ibv_flow_spec_tcp_udp *>(flow_spec_eth + 1);
    flow_attr->size = flow_attr_total_size;
    flow_attr->priority = 0;
    flow_attr->num_of_specs = 2;
    flow_attr->port = net_param.ib_port;
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

    flow_spec_udp->type = IBV_FLOW_SPEC_UDP;
    flow_spec_udp->size = sizeof(ibv_flow_spec_tcp_udp);
    flow_spec_udp->val.dst_port = htons(FLOW_UDP_DST_PORT);
    flow_spec_udp->mask.dst_port = 0xFFFF;

    ibv_flow *main_flow = ibv_create_flow(main_qp->qp, flow_attr);
    assert(main_flow);
    free(header_buff);

    vector<thread> threads(NUM_THREADS);
    for (int i = 0;i < NUM_THREADS;i++) {
        int now_index = get_cpu_index_with_numa(i + CORE_OFFSET, net_param.numa_node);
        if (IS_SERVER) {
            threads[i] = thread(sub_recv_server, now_index, wq_handlers[i]);
        } else {
            threads[i] = thread(sub_send_server, now_index, wq_handlers[i]);
        }
        set_cpu_with_numa(threads[i], i + CORE_OFFSET, net_param.numa_node);
    }

    for (int i = 0;i < NUM_THREADS;i++) {
        threads[i].join();
    }

    assert(ibv_destroy_flow(main_flow) == 0);
    assert(ibv_destroy_qp(main_qp->qp) == 0);
    assert(ibv_destroy_rwq_ind_table(rwq_ind_table) == 0);

    for (int i = 0;i < NUM_THREADS;i++) {
        free(wq_handlers[i]->recv_sge_list);
        free(wq_handlers[i]->recv_wr);
        free(wq_handlers[i]->recv_bar_wr);

        free(wq_handlers[i]->send_sge_list);
        free(wq_handlers[i]->send_wr);
        free(wq_handlers[i]->send_bar_wr);

        assert(ibv_dereg_mr(wq_handlers[i]->mr) == 0);
        assert(ibv_destroy_wq(wq_handlers[i]->recv_wq) == 0);
        assert(ibv_destroy_qp(wq_handlers[i]->send_qp) == 0);
        assert(ibv_destroy_cq(wq_handlers[i]->recv_cq) == 0);
        assert(ibv_destroy_cq(wq_handlers[i]->send_cq) == 0);

        free(wq_handlers[i]);
    }
    delete[]wq_handlers;

    assert(ibv_dealloc_pd(main_pd) == 0);
    free(main_qp);

    for (int i = 0;i < net_param.num_contexts;i++) {
        ibv_close_device(net_param.contexts[i]);
    }
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

    roce_init(net_param, 1);
    benchmark(net_param);
}