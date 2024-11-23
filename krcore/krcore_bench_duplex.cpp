#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <numaif.h>
#include <numa.h>
#include <fcntl.h>
#include <algorithm>

#include <gflags/gflags.h>
#include <hdr/hdr_histogram.h>
#include "./rdma_krcore.h"
#include <x86intrin.h>


DEFINE_int32(iterations, 1000, "iterations");
DEFINE_int32(packSize, 1024, "packSize");
DEFINE_int32(threads, 1, "num_threads");
DEFINE_int32(nodeId, 0, "nodeId");
DEFINE_string(serverIp, "", "serverIp");
DEFINE_int32(coreOffset, 0, "coreOffset");
DEFINE_int32(numaNode, 0, "numaNode");
DEFINE_int32(port, 6666, "bind_port");
DEFINE_uint64(batch_size, 1, "requeset batch_size");
DEFINE_uint64(outstanding, 64, "outstanding request");
std::atomic<bool> stop_flag = false;
unsigned char client_mac[6] = { 0x98,0x03,0x9b,0xca,0x48,0x38 };
unsigned char server_mac[6] = { 0x98,0x03,0x9b,0xc7,0xc8,0x18 };
void ctrl_c_handler(int) { stop_flag = true; }
hdr_histogram *latency_hist = nullptr;
double scale_value = 10;
std::mutex IO_LOCK;
const char *krcoreinode = "/dev/krcore";

class NetParam {
public:
    int nodeId;
    std::string serverIp;
    int numNodes;
    int sockfd[2];
    int sock_port;
};
class OffsetHandler {
private:
    int max_num;
    int step_size;
    int buf_offset;
    size_t cur;

public:
    OffsetHandler() {
        cur = 0;
    }

    OffsetHandler(int max_num, int step_size, int buf_offset):max_num(max_num), step_size(step_size), buf_offset(buf_offset) {
        cur = 0;
    }
    void init(int max_num, int step_size, int buf_offset) {
        cur = 0;
        this->max_num = max_num;
        this->step_size = step_size;
        this->buf_offset = buf_offset;
    }
    size_t step() {
        size_t ret = offset();
        cur += 1;
        return ret;
    }
    size_t step(int step) {
        size_t ret = offset();
        cur += step;
        return ret;
    }
    size_t offset() {
        return (cur % max_num) * step_size + buf_offset;
    }
    size_t index() {
        return cur;
    }
    int index_mod() {
        return cur % max_num;
    }
};
#define ROUND_UP(value, alignment) (((value) + (alignment) - 1) & ~((alignment) - 1))

static double get_tsc_freq_per_ns() {
    // MUST BE CHANGE BY 
    // sudo journalctl -k --grep '^tsc:'  | cut -d' ' -f5-
    return 2.2;
}

static size_t get_tsc() {
    return __rdtsc();
}

void socket_init(NetParam &net_param) {
    if (net_param.sock_port == 0) {
        net_param.sock_port = 6666;
    }

    if (net_param.nodeId == 0) {
        printf("\n************************************\n");
        printf("* Waiting for client to connect... *\n");
        printf("************************************\n");
        fflush(stdout);
        addrinfo hints{};
        addrinfo *server_address{ nullptr };
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        assert(getaddrinfo(NULL, std::to_string(net_param.sock_port).c_str(), &hints, &server_address) >= 0);
        auto sockfd = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);
        assert(sockfd > 0);
        int reuse = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        assert(bind(sockfd, server_address->ai_addr, server_address->ai_addrlen) == 0);
        free(server_address);
        assert(listen(sockfd, 128) == 0);
        for (int i = 0;i < net_param.numNodes - 1;i++) {//numNodes-1 nodes
            int connfd = accept(sockfd, NULL, 0);
            assert(connfd >= 0);
            int nodeId = 0;
            assert(read(connfd, static_cast<void *>(&nodeId), sizeof(nodeId)) == sizeof(nodeId));
            printf("connected by %d\n", nodeId);
            fflush(stdout);
            net_param.sockfd[nodeId] = connfd;
        }
    } else {//client
        sleep(1);
        addrinfo hints{};
        hints.ai_socktype = SOCK_STREAM;
        addrinfo *server_address{ nullptr };
        assert(getaddrinfo(net_param.serverIp.c_str(), std::to_string(net_param.sock_port).c_str(), &hints, &server_address) >= 0);
        int sockfd = socket(server_address->ai_family, server_address->ai_socktype, server_address->ai_protocol);
        assert(sockfd > 0);
        assert(connect(sockfd, server_address->ai_addr, server_address->ai_addrlen) == 0);
        freeaddrinfo(server_address);
        int nodeId = net_param.nodeId;
        assert(send(sockfd, static_cast<void *>(&nodeId), sizeof(nodeId), 0) == sizeof(nodeId));
        net_param.sockfd[0] = sockfd;
    }
}
void exchange_data(NetParam &net_param, PingPongInfo *local_info, PingPongInfo *remote_info) {
    printf("exchange data size:%ld\n", sizeof(PingPongInfo));
    size_t dummy;
    (void)dummy;
    if (net_param.nodeId == 0) {
        for (int i = 1;i < net_param.numNodes;i++) {
            dummy = read(net_param.sockfd[i], remote_info, sizeof(PingPongInfo));
        }
        for (int i = 1;i < net_param.numNodes;i++) {
            dummy = write(net_param.sockfd[i], local_info, sizeof(PingPongInfo));
        }
    } else {
        dummy = write(net_param.sockfd[0], local_info, sizeof(PingPongInfo));
        dummy = read(net_param.sockfd[0], remote_info, sizeof(PingPongInfo));
    }
}

void set_cpu(std::thread &t, int cpu_index) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_index, &cpuset);
    int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        printf("%-20s : %d", "Error calling pthread_setaffinity_np", rc);
    }
}
void set_cpu_with_numa(std::thread &t, int cpu_index, int numa_node) {
    std::vector<size_t> cpu_cores_list;
    auto num_lcores = static_cast<size_t>(numa_num_configured_cpus());

    for (size_t i = 0; i < num_lcores; i++) {
        if (numa_node == numa_node_of_cpu(static_cast<int>(i))) {
            cpu_cores_list.push_back(i);
        }
    }

    if (static_cast<size_t>(cpu_index) >= cpu_cores_list.size()) {
        printf("cpu_index[%d] >= cpu_cores_list.size()[%ld]", cpu_index, cpu_cores_list.size());
    }

    set_cpu(t, static_cast<int>(cpu_cores_list[cpu_index]));
}

int get_cpu_index_with_numa(int cpu_index, int numa_node) {
    std::vector<size_t> cpu_cores_list;
    auto num_lcores = static_cast<size_t>(numa_num_configured_cpus());

    for (size_t i = 0; i < num_lcores; i++) {
        if (numa_node == numa_node_of_cpu(static_cast<int>(i))) {
            cpu_cores_list.push_back(i);
        }
    }

    if (static_cast<size_t>(cpu_index) >= cpu_cores_list.size()) {
        printf("cpu_index[%d] >= cpu_cores_list.size()[%ld]", cpu_index, cpu_cores_list.size());
    }
    return static_cast<int>(cpu_cores_list[cpu_index]);
}

void wait_scheduling(int thread_index, std::mutex &IO_LOCK) {
    while (thread_index != sched_getcpu()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));//wait set affinity success
    }
    std::lock_guard<std::mutex> guard(IO_LOCK);
    printf("Thread [%2d] has been moved to core [%2d]", thread_index, sched_getcpu());
}


void krcore_post_send(int krcore_fd, size_t offset, size_t batch_size, int length) {
    struct KRCORE_IOC_POST_SEND_PARAMS post_send_params;
    post_send_params.offset = offset;
    post_send_params.batch_size = batch_size;
    post_send_params.length = length;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POST_SEND, &post_send_params);
    if (retcode != 0 || post_send_params.success_send_cnt != batch_size) {
        printf("ioctl KRCORE_IOC_POST_SEND failed\n");
        exit(1);
    }
}

void krcore_post_recv(int krcore_fd, size_t offset, size_t batch_size, int length) {
    struct KRCORE_IOC_POST_RECV_PARAMS post_recv_params;
    post_recv_params.offset = offset;
    post_recv_params.batch_size = batch_size;
    post_recv_params.length = length;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POST_RECV, &post_recv_params);
    if (retcode != 0 || post_recv_params.success_post_cnt != batch_size) {
        printf("ioctl KRCORE_IOC_POST_RECV failed\n");
        exit(1);
    }
}

void krcore_poll_send_cq(int krcore_fd, int max_poll_num, size_t &actual_poll_num) {
    struct KRCORE_IOC_POLL_SEND_CQ_PARAMS poll_send_cq_params;
    poll_send_cq_params.max_poll_num = max_poll_num;
    poll_send_cq_params.actual_poll_num = 0;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POLL_SEND_CQ, &poll_send_cq_params);
    if (retcode != 0) {
        printf("ioctl KRCORE_IOC_POLL_SEND_CQ failed\n");
        exit(1);
    }
    actual_poll_num = poll_send_cq_params.actual_poll_num;
}

void krcore_poll_recv_cq(int krcore_fd, int max_poll_num, size_t &actual_poll_num) {
    struct KRCORE_IOC_POLL_RECV_CQ_PARAMS poll_recv_cq_params;
    poll_recv_cq_params.max_poll_num = max_poll_num;
    poll_recv_cq_params.actual_poll_num = 0;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POLL_RECV_CQ, &poll_recv_cq_params);
    if (retcode != 0) {
        printf("ioctl KRCORE_IOC_POLL_RECV_CQ failed\n");
        exit(1);
    }
    actual_poll_num = poll_recv_cq_params.actual_poll_num;
}

void sub_task_server(int thread_index, int krcore_fd, void *user_local_buf) {
    size_t send_recv_buf_size = KRCORE_ALLOC_SIZE / 2;

    OffsetHandler send(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
    OffsetHandler send_comp(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
    OffsetHandler recv(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
    OffsetHandler recv_comp(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);


    size_t rx_depth = KRCORE_RX_DEPTH;

    size_t ops = FLAGS_iterations * (send_recv_buf_size / FLAGS_packSize);
    ops = ROUND_UP(ops, FLAGS_batch_size);

    for (size_t i = 0;i < rx_depth;i++) {
        krcore_post_recv(krcore_fd, recv.offset(), 1, FLAGS_packSize);
        recv.step();
    }

    int done = 0;
    struct timespec begin_time, end_time;
    size_t actual_poll_send_num, actual_poll_recv_num;
    begin_time.tv_nsec = 0;
    begin_time.tv_sec = 0;

    while (!done && !stop_flag) {
        krcore_poll_recv_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, actual_poll_recv_num);
        if (actual_poll_recv_num != 0 && begin_time.tv_sec == 0) {
            clock_gettime(CLOCK_MONOTONIC, &begin_time);
        }
        for (size_t i = 0;i < actual_poll_recv_num;i++) {
            if (recv.index() < ops) {
                if (recv_comp.index() % FLAGS_batch_size == FLAGS_batch_size - 1) {
                    krcore_post_recv(krcore_fd, recv.offset(), FLAGS_batch_size, FLAGS_packSize);
                    recv.step(FLAGS_batch_size);
                }
            }
            recv_comp.step();
        }
        if (actual_poll_recv_num > 0) {
            size_t tmp_recv_num = actual_poll_recv_num;
            while (tmp_recv_num > 0) {
                int now_send_num = std::min(tmp_recv_num, FLAGS_batch_size);
                krcore_post_send(krcore_fd, send.offset(), now_send_num, FLAGS_packSize);
                send.step(now_send_num);
                tmp_recv_num -= now_send_num;
            }
        }
        krcore_poll_send_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, actual_poll_send_num);
        for (size_t i = 0;i < actual_poll_send_num;i++) {
            send_comp.step();
        }
        if (recv_comp.index() >= ops && send_comp.index() >= ops) {
            done = 1;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double duration = (end_time.tv_sec - begin_time.tv_sec) + (end_time.tv_nsec - begin_time.tv_nsec) / 1e9;
    double speed = 8.0 * send_comp.index() * FLAGS_packSize / 1000 / 1000 / 1000 / duration;

    std::lock_guard<std::mutex> guard(IO_LOCK);
    printf("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs", thread_index, duration, speed);
}

void sub_task_client(int thread_index, int krcore_fd, void *user_local_buf) {
    size_t send_recv_buf_size = KRCORE_ALLOC_SIZE / 2;
    OffsetHandler send(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
    OffsetHandler send_comp(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
    OffsetHandler recv(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
    OffsetHandler recv_comp(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);


    size_t tx_depth = FLAGS_outstanding;//handler->tx_depth;
    size_t rx_depth = KRCORE_RX_DEPTH;

    size_t ops = FLAGS_iterations * (send_recv_buf_size / FLAGS_packSize);
    ops = ROUND_UP(ops, FLAGS_batch_size);

    std::vector<size_t>timers(128);
    size_t timer_head = 0, timer_tail = 0;

    for (size_t i = 0;i < rx_depth;i++) {
        krcore_post_recv(krcore_fd, recv.offset(), 1, FLAGS_packSize);
        recv.step();
    }

    for (size_t i = 0;i < tx_depth;i++) {
        krcore_post_send(krcore_fd, send.offset(), 1, FLAGS_packSize);
        timers[timer_head] = get_tsc();
        timer_head = (timer_head + 1) % 128;
        send.step();
    }

    int done = 0;
    struct timespec begin_time, end_time;
    size_t actual_poll_send_num, actual_poll_recv_num;
    begin_time.tv_nsec = 0;
    begin_time.tv_sec = 0;

    while (!done && !stop_flag) {
        krcore_poll_recv_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, actual_poll_recv_num);
        if (actual_poll_recv_num != 0 && begin_time.tv_sec == 0) {
            clock_gettime(CLOCK_MONOTONIC, &begin_time);
        }
        for (size_t i = 0;i < actual_poll_recv_num;i++) {
            hdr_record_value_atomic(latency_hist, (get_tsc() - timers[timer_tail]) * 10);
            // printf("recv_comp:%ld\n", recv_comp.index());
            timer_tail = (timer_tail + 1) % 128;
        }
        for (size_t i = 0;i < actual_poll_recv_num;i++) {
            if (recv.index() < ops) {
                if (recv_comp.index() % FLAGS_batch_size == FLAGS_batch_size - 1) {
                    krcore_post_recv(krcore_fd, recv.offset(), FLAGS_batch_size, FLAGS_packSize);
                    recv.step(FLAGS_batch_size);
                }
            }
            recv_comp.step();
        }

        krcore_poll_send_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, actual_poll_send_num);
        for (size_t i = 0;i < actual_poll_send_num;i++) {
            send_comp.step();
        }

        if (send.index() < ops && send.index() - recv_comp.index() < tx_depth) {
            size_t now_send_num = std::min(ops - send.index(), tx_depth - (send.index() - recv_comp.index()));
            while (now_send_num > 0) {
                size_t tmp_send_num = std::min(now_send_num, FLAGS_batch_size);
                krcore_post_send(krcore_fd, send.offset(), tmp_send_num, FLAGS_packSize);
                for (size_t i = 0;i < tmp_send_num;i++) {
                    timers[timer_head] = get_tsc();
                    timer_head = (timer_head + 1) % 128;
                }
                send.step(tmp_send_num);
                now_send_num -= tmp_send_num;
            }
        }

        if (recv_comp.index() >= ops && send_comp.index() >= ops) {
            done = 1;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double duration = (end_time.tv_sec - begin_time.tv_sec) + (end_time.tv_nsec - begin_time.tv_nsec) / 1e9;
    double speed = 8.0 * send_comp.index() * FLAGS_packSize / 1000 / 1000 / 1000 / duration;

    std::lock_guard<std::mutex> guard(IO_LOCK);
    printf("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs", thread_index, duration, speed);
}

void sub_task(int thread_index) {
    sleep(2);
    wait_scheduling(thread_index, IO_LOCK);

    NetParam net_param;
    net_param.numNodes = 2;
    net_param.nodeId = FLAGS_nodeId;
    net_param.serverIp = FLAGS_serverIp;
    net_param.sock_port = FLAGS_port + thread_index;
    socket_init(net_param);

    void *user_local_buf = malloc(KRCORE_ALLOC_SIZE);
    int fd = open(krcoreinode, O_RDWR | O_CLOEXEC);
    if (fd == -1) {
        printf("thread %d open %s failed\n", thread_index, krcoreinode);
    }

    struct KRCORE_IOC_CREATE_QP_PARAMS create_qp_params;
    create_qp_params.user_buf = reinterpret_cast<size_t>(user_local_buf);
    create_qp_params.batch_size = FLAGS_batch_size;
    int retcode = ioctl(fd, KRCORE_IOC_CREATE_QP, &create_qp_params);
    if (retcode != 0) {
        printf("thread %d ioctl KRCORE_IOC_CREATE_QP %s failed\n", thread_index, krcoreinode);
    }

    struct KRCORE_IOC_INIT_QP_PARAMS init_qp_params;
    if (FLAGS_nodeId == 0) {
        memcpy(create_qp_params.info.mac, server_mac, 6);
    } else {
        memcpy(create_qp_params.info.mac, client_mac, 6);
    }
    exchange_data(net_param, &create_qp_params.info, &init_qp_params.info);
    printf("local qpn = %d remote qpn = %d\n", create_qp_params.info.qpn, init_qp_params.info.qpn);
    retcode = ioctl(fd, KRCORE_IOC_INIT_QP, &init_qp_params);
    if (retcode != 0) {
        printf("thread %d ioctl KRCORE_IOC_INIT_QP %s failed\n", thread_index, krcoreinode);
    }

    if (FLAGS_nodeId == 0) {
        sub_task_server(thread_index, fd, user_local_buf);
    } else {
        sub_task_client(thread_index, fd, user_local_buf);
    }

    struct KRCORE_IOC_FREE_QP_PARAMS free_qp_params;
    retcode = ioctl(fd, KRCORE_IOC_FREE_QP, &free_qp_params);
    if (retcode != 0 || free_qp_params.success != 1) {
        printf("thread %d ioctl KRCORE_IOC_FREE_QP %s failed\n", thread_index, krcoreinode);
    }

    free(user_local_buf);

    close(fd);
}

void benchmark() {
    std::vector<std::thread> threads(FLAGS_threads);
    for (int i = 0;i < FLAGS_threads;i++) {
        int now_index = get_cpu_index_with_numa(i + FLAGS_coreOffset, FLAGS_numaNode);
        threads[i] = std::thread(sub_task, now_index);
        set_cpu_with_numa(threads[i], i + FLAGS_coreOffset, FLAGS_numaNode);
    }

    for (int i = 0;i < FLAGS_threads;i++) {
        threads[i].join();
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, ctrl_c_handler);
    signal(SIGTERM, ctrl_c_handler);

    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (FLAGS_nodeId != 0) {
        hdr_init(1000, 50000000, 3, &latency_hist);
    }

    benchmark();

    if (FLAGS_nodeId != 0) {
        hdr_percentiles_print(latency_hist, stdout, 5, 10 * get_tsc_freq_per_ns(), CLASSIC);
        hdr_close(latency_hist);
    }

}