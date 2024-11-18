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

int OUTSTANDING = 48;
std::atomic<bool> stop_flag = false;

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
            printf("connected by %d", nodeId);
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
    printf("exchange data size:%ld", sizeof(PingPongInfo));
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

void sub_task_server(int thread_index) {
}

void sub_task_client(int thread_index) {
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
    int retcode = ioctl(fd, KRCORE_IOC_CREATE_QP, &create_qp_params);
    if (retcode != 0) {
        printf("thread %d ioctl KRCORE_IOC_CREATE_QP %s failed\n", thread_index, krcoreinode);
    }

    struct KRCORE_IOC_INIT_QP_PARAMS init_qp_params;
    exchange_data(net_param, &create_qp_params.info, &init_qp_params.info);

    retcode = ioctl(fd, KRCORE_IOC_INIT_QP, &init_qp_params);
    if (retcode != 0) {
        printf("thread %d ioctl KRCORE_IOC_INIT_QP %s failed\n", thread_index, krcoreinode);
    }

    if (FLAGS_nodeId == 0) {
        sub_task_server(thread_index);
    } else {
        sub_task_client(thread_index);
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