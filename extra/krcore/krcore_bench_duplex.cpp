#include "krcore_bench_duplex.h"

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
void exchange_data(NetParam &net_param, PingPongInfo *local_info, PingPongInfo *remote_info, size_t qp_per_core) {
    printf("exchange data size:%ld\n", sizeof(PingPongInfo) * qp_per_core);
    size_t dummy;
    (void)dummy;
    if (net_param.nodeId == 0) {
        for (int i = 1;i < net_param.numNodes;i++) {
            dummy = read(net_param.sockfd[i], remote_info, sizeof(PingPongInfo) * qp_per_core);
        }
        for (int i = 1;i < net_param.numNodes;i++) {
            dummy = write(net_param.sockfd[i], local_info, sizeof(PingPongInfo) * qp_per_core);
        }
    } else {
        dummy = write(net_param.sockfd[0], local_info, sizeof(PingPongInfo) * qp_per_core);
        dummy = read(net_param.sockfd[0], remote_info, sizeof(PingPongInfo) * qp_per_core);
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




void sub_task_server(int thread_index, int krcore_fd, void **user_local_buf) {
    size_t send_recv_buf_size = KRCORE_ALLOC_SIZE / 2;

    OffsetHandler send[KRCORE_MAX_QP_PER_CORE];
    OffsetHandler send_comp[KRCORE_MAX_QP_PER_CORE];
    OffsetHandler recv[KRCORE_MAX_QP_PER_CORE];
    OffsetHandler recv_comp[KRCORE_MAX_QP_PER_CORE];
    for (size_t i = 0;i < KRCORE_MAX_QP_PER_CORE;i++) {
        send[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        send_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        recv[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
        recv_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
    }

    size_t rx_depth = KRCORE_RX_DEPTH;

    for (size_t qp_id = 0; qp_id < FLAGS_qp_per_core; qp_id++) {
        for (size_t i = 0;i < rx_depth;i++) {
            krcore_post_recv(krcore_fd, recv[qp_id].offset(), qp_id, 1, FLAGS_packSize);
            recv[qp_id].step();
        }
    }

    size_t actual_poll_send_num, actual_poll_recv_num;
    size_t batch_size = FLAGS_batch_size;

    // sync with other server threads
    send_sync++;
    while (send_sync != FLAGS_threads) {
    }

    while (!stop_flag) {
        for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
            krcore_poll_recv_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, qp_id, actual_poll_recv_num);

            for (size_t i = 0;i < actual_poll_recv_num;i++) {
                if (recv_comp[qp_id].index() % batch_size == batch_size - 1) {
                    krcore_post_recv(krcore_fd, recv[qp_id].offset(), qp_id, batch_size, FLAGS_packSize);
                    recv[qp_id].step(batch_size);
                }
                recv_comp[qp_id].step();
            }

            if (recv_comp[qp_id].index() - send[qp_id].index() >= batch_size) {
                krcore_post_send(krcore_fd, send[qp_id].offset(), qp_id, batch_size, FLAGS_packSize);
                send[qp_id].step(batch_size);
            }

            krcore_poll_send_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, qp_id, actual_poll_send_num);
            for (size_t i = 0;i < actual_poll_send_num;i++) {
                send_comp[qp_id].step();
            }
        }
    }

    for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
        while (send_comp[qp_id].index() < send[qp_id].index()) {
            krcore_poll_send_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, qp_id, actual_poll_send_num);
            for (size_t i = 0;i < actual_poll_send_num;i++) {
                send_comp[qp_id].step();
            }
        }
    }

}

void sub_task_client(int thread_index, int krcore_fd, void **user_local_buf) {
    size_t send_recv_buf_size = KRCORE_ALLOC_SIZE / 2;
    OffsetHandler send[KRCORE_MAX_QP_PER_CORE];
    OffsetHandler send_comp[KRCORE_MAX_QP_PER_CORE];
    OffsetHandler recv[KRCORE_MAX_QP_PER_CORE];
    OffsetHandler recv_comp[KRCORE_MAX_QP_PER_CORE];
    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        send[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        send_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        recv[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
        recv_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
    }

    size_t tx_depth = FLAGS_outstanding;//handler->tx_depth;
    size_t rx_depth = KRCORE_RX_DEPTH;

    size_t ops = FLAGS_iterations * (send_recv_buf_size / FLAGS_packSize);
    ops = ROUND_UP(ops, FLAGS_batch_size);

    std::vector<std::vector<size_t>>timers(FLAGS_qp_per_core, std::vector<size_t>(128, 0));
    std::vector<size_t>timer_head(FLAGS_qp_per_core);
    std::vector<size_t>timer_tail(FLAGS_qp_per_core);

    for (size_t qp_id = 0; qp_id < FLAGS_qp_per_core; qp_id++) {
        for (size_t i = 0;i < rx_depth;i++) {
            krcore_post_recv(krcore_fd, recv[qp_id].offset(), qp_id, 1, FLAGS_packSize);
            recv[qp_id].step();
        }
    }

    send_sync++;
    while (send_sync != FLAGS_threads) {
    }

    for (size_t i = 0;i < tx_depth;i++) {
        for (size_t qp_id = 0; qp_id < FLAGS_qp_per_core;qp_id++) {
            krcore_post_send(krcore_fd, send[qp_id].offset(), qp_id, 1, FLAGS_packSize);
            // timers[qp_id][timer_head[qp_id]] = get_tsc();
            // timer_head[qp_id] = (timer_head[qp_id] + 1) % 128;
            send[qp_id].step();
        }
    }

    int done = 0;
    struct timespec begin_time, end_time;
    size_t actual_poll_send_num, actual_poll_recv_num;
    begin_time.tv_nsec = 0;
    begin_time.tv_sec = 0;

    size_t total_finish = 0;
    size_t batch_size = FLAGS_batch_size;

    while (!done && !stop_flag) {
        for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
            krcore_poll_recv_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, qp_id, actual_poll_recv_num);
            if (actual_poll_recv_num != 0 && begin_time.tv_sec == 0) {
                clock_gettime(CLOCK_MONOTONIC, &begin_time);
            }
            for (size_t i = 0;i < actual_poll_recv_num;i++) {
                // hdr_record_value_atomic(latency_hist, (get_tsc() - timers[qp_id][timer_tail[qp_id]]) * 10);
                // printf("recv_comp:%ld\n", recv_comp[qp_id].index());
                // timer_tail[qp_id] = (timer_tail[qp_id] + 1) % 128;
            }
            for (size_t i = 0;i < actual_poll_recv_num;i++) {
                if (recv[qp_id].index() < ops) {
                    if (recv_comp[qp_id].index() % batch_size == batch_size - 1) {
                        krcore_post_recv(krcore_fd, recv[qp_id].offset(), qp_id, batch_size, FLAGS_packSize);
                        recv[qp_id].step(batch_size);
                    }
                }
                recv_comp[qp_id].step();
                total_finish++;
            }

            krcore_poll_send_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, qp_id, actual_poll_send_num);
            for (size_t i = 0;i < actual_poll_send_num;i++) {
                send_comp[qp_id].step();
            }

            if (send[qp_id].index() < ops && send[qp_id].index() - recv_comp[qp_id].index() <= tx_depth - batch_size) {
                size_t now_send_num = std::min(ops - send[qp_id].index(), batch_size);
                krcore_post_send(krcore_fd, send[qp_id].offset(), qp_id, now_send_num, FLAGS_packSize);
                send[qp_id].step(now_send_num);
            }

            if (total_finish >= ops) {
                done = 1;
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double duration = (end_time.tv_sec - begin_time.tv_sec) + (end_time.tv_nsec - begin_time.tv_nsec) / 1e9;
    double speed = 8.0 * total_finish * FLAGS_packSize / 1000 / 1000 / 1000 / duration;

    for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
        while (!stop_flag && send_comp[qp_id].index() < send[qp_id].index()) {
            krcore_poll_send_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, qp_id, actual_poll_send_num);
            for (size_t i = 0;i < actual_poll_send_num;i++) {
                send_comp[qp_id].step();
            }
        }
    }

    std::lock_guard<std::mutex> guard(IO_LOCK);
    printf("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs\n", thread_index, duration, speed);
    total_bw = total_bw + speed;
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


    void *user_local_buf[KRCORE_MAX_QP_PER_CORE];
    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        user_local_buf[i] = malloc(KRCORE_ALLOC_SIZE);
    }
    int fd = open(krcoreinode, O_RDWR | O_CLOEXEC);
    if (fd == -1) {
        printf("thread %d open %s failed\n", thread_index, krcoreinode);
    }

    struct KRCORE_IOC_CREATE_QP_PARAMS create_qp_params;
    create_qp_params.qp_per_core = FLAGS_qp_per_core;
    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        create_qp_params.user_buf[i] = reinterpret_cast<size_t>(user_local_buf[i]);
    }
    create_qp_params.batch_size = FLAGS_batch_size;
    int retcode = ioctl(fd, KRCORE_IOC_CREATE_QP, &create_qp_params);
    if (retcode != 0) {
        printf("thread %d ioctl KRCORE_IOC_CREATE_QP %s failed\n", thread_index, krcoreinode);
    }

    struct KRCORE_IOC_INIT_QP_PARAMS init_qp_params;
    init_qp_params.qp_per_core = FLAGS_qp_per_core;
    if (FLAGS_nodeId == 0) {
        for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
            memcpy(create_qp_params.info[i].mac, server_mac, 6);
        }
    } else {
        for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
            memcpy(create_qp_params.info[i].mac, client_mac, 6);
        }
    }
    exchange_data(net_param, create_qp_params.info, init_qp_params.info, FLAGS_qp_per_core);
    retcode = ioctl(fd, KRCORE_IOC_INIT_QP, &init_qp_params);
    if (retcode != 0) {
        printf("thread %d ioctl KRCORE_IOC_INIT_QP %s failed\n", thread_index, krcoreinode);
    }

    if (FLAGS_nodeId == 0) {
        if (thread_index == FLAGS_coreOffset) {
            sub_task_latency_server(thread_index, fd, user_local_buf);
        } else {
            sub_task_server(thread_index, fd, user_local_buf);
        }
    } else {
        if (thread_index == FLAGS_coreOffset) {
            sub_task_latency_client(thread_index, fd, user_local_buf);
        } else {
            sub_task_client(thread_index, fd, user_local_buf);
        }
    }

    struct KRCORE_IOC_FREE_QP_PARAMS free_qp_params;
    free_qp_params.qp_per_core = FLAGS_qp_per_core;
    retcode = ioctl(fd, KRCORE_IOC_FREE_QP, &free_qp_params);
    if (retcode != 0 || free_qp_params.success != FLAGS_qp_per_core) {
        printf("thread %d ioctl KRCORE_IOC_FREE_QP %s failed\n", thread_index, krcoreinode);
    }

    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        free((void *)user_local_buf[i]);
    }

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
        hdr_init(1000 * scale_value, 50000000 * scale_value, 3, &latency_hist);
    }
    assert(FLAGS_qp_per_core <= KRCORE_MAX_QP_PER_CORE);

    benchmark();

    if (FLAGS_nodeId != 0) {
        printf("Total throughput: %f Gbps\n", total_bw.load());
    }
    // if (FLAGS_nodeId != 0) {
    //     hdr_percentiles_print(latency_hist, stdout, 5, 10 * get_tsc_freq_per_ns(), CLASSIC);
    //     hdr_close(latency_hist);
    // }
}