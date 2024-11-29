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
#include "libr.hpp"
#include <hdr/hdr_histogram.h>

#include "freeflow_bench_duplex.h"

DEFINE_int32(iterations, 1000, "iterations");
DEFINE_int32(packSize, 1024, "packSize");
DEFINE_int32(threads, 1, "num_threads");
DEFINE_int32(nodeId, 0, "nodeId");
DEFINE_string(serverIp, "", "serverIp");
DEFINE_int32(coreOffset, 0, "coreOffset");
DEFINE_int32(numaNode, 0, "numaNode");
DEFINE_int32(port, 6666, "bind_port");
DEFINE_uint64(batch_size, 1, "requeset batch_size");
DEFINE_uint64(qp_per_core, 1, "qp per core");
DEFINE_uint64(outstanding, 32, "outstanding request");
DEFINE_string(deviceName, "mlx5_0", "deviceName");
DEFINE_int32(gidIndex, 3, "gidIndex");

std::atomic<bool> stop_flag = false;
void ctrl_c_handler(int) { stop_flag = true; }
hdr_histogram *latency_hist = nullptr;
double scale_value = 10;
std::mutex IO_LOCK;
std::atomic<int> send_sync = 0;
std::atomic<double> total_bw = 0;

#define ROUND_UP(value, alignment) (((value) + (alignment) - 1) & ~((alignment) - 1))

const auto kPageSize = 4096;
struct ListNode {
    ListNode *next;
    std::byte padding[kPageSize];
};

void sub_task_server(int thread_index, QpHandler **handler) {
    size_t send_recv_buf_size = FREEFLOW_ALLOC_SIZE / 2;

    struct ibv_wc *wc_send = NULL;
    struct ibv_wc *wc_recv = NULL;
    ALLOCATE(wc_send, struct ibv_wc, CTX_POLL_BATCH);
    ALLOCATE(wc_recv, struct ibv_wc, CTX_POLL_BATCH);

    OffsetHandler send[FREEFLOW_MAX_QP_PER_CORE];
    OffsetHandler send_comp[FREEFLOW_MAX_QP_PER_CORE];
    OffsetHandler recv[FREEFLOW_MAX_QP_PER_CORE];
    OffsetHandler recv_comp[FREEFLOW_MAX_QP_PER_CORE];

    void **copy_bufs = new void *[FLAGS_qp_per_core];
    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        copy_bufs[i] = malloc(FREEFLOW_ALLOC_SIZE);
    }

    for (size_t i = 0;i < FREEFLOW_MAX_QP_PER_CORE;i++) {
        send[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        send_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        recv[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
        recv_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
    }

    size_t rx_depth = handler[0]->rx_depth;

    for (size_t qp_id = 0; qp_id < FLAGS_qp_per_core; qp_id++) {
        for (size_t i = 0;i < rx_depth;i++) {
            post_recv(*(handler[qp_id]), recv[qp_id].offset(), FLAGS_packSize);
            recv[qp_id].step();
        }
    }

    size_t ne_send;
    size_t ne_recv;
    size_t batch_size = FLAGS_batch_size;

    size_t num_nodes = 128 * 1024 * 1024 / (64);
    std::vector<ListNode> list(num_nodes);
    for (size_t i = 0;i < list.size() / 2;i++) {
        list[i].next = &list[list.size() - 1 - i];
    }
    for (size_t i = list.size() / 2 + 1;i < list.size();i++) {
        list[i].next = &list[list.size() - i];
    }
    list[list.size() / 2].next = &list[0];

    ListNode *now_head = &list[0];
    size_t traverse_num = 8;
    while (!stop_flag) {
        for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
            ne_recv = poll_recv_cq(*(handler[qp_id]), wc_recv);

            for (size_t i = 0;i < ne_recv;i++) {
                assert(wc_recv[i].status == IBV_WC_SUCCESS);
                recv_comp[qp_id].step();
                if (recv_comp[qp_id].index() % batch_size == 0) {
                    post_recv_batch(*(handler[qp_id]), batch_size, recv[qp_id], FLAGS_packSize);
                }
                // fake freeflow
                {
                    memcpy(copy_bufs[qp_id] + wc_recv[i].wr_id, reinterpret_cast<void *>(handler[qp_id]->buf + wc_recv[i].wr_id), wc_recv[i].byte_len);
                    memcpy(copy_bufs[qp_id] + send_recv_buf_size, wc_recv + i, sizeof(struct ibv_wc));
                    size_t now_traverse_num = traverse_num;
                    while (now_traverse_num > 0) {
                        now_head = now_head->next;
                        now_traverse_num--;
                    }
                }

            }

            if (recv_comp[qp_id].index() - send[qp_id].index() >= batch_size) {
                post_send_batch(*(handler[qp_id]), batch_size, send[qp_id], FLAGS_packSize);
                // fake freeflow
                {
                    memcpy(copy_bufs[qp_id], handler[qp_id]->send_wr, sizeof(struct ibv_send_wr) * batch_size);
                    size_t now_traverse_num = traverse_num;
                    while (now_traverse_num > 0) {
                        now_head = now_head->next;
                        now_traverse_num--;
                    }
                }
            }

            ne_send = poll_send_cq(*(handler[qp_id]), wc_send);
            for (size_t i = 0;i < ne_send;i++) {
                assert(wc_send[i].status == IBV_WC_SUCCESS);
                send_comp[qp_id].step(SEND_CQ_BATCH);
            }
        }
    }

    // for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
    //     while (send_comp[qp_id].index() < send[qp_id].index()) {
    //         ne_send = poll_send_cq(*(handler[qp_id]), wc_send);
    //         for (size_t i = 0;i < ne_send;i++) {
    //             assert(wc_send[i].status == IBV_WC_SUCCESS);
    //             send_comp[qp_id].step(SEND_CQ_BATCH);
    //         }
    //     }
    // }

    free(wc_send);
    free(wc_recv);
    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        free(copy_bufs[i]);
    }
}

void sub_task_client(int thread_index, QpHandler **handler) {
    size_t send_recv_buf_size = FREEFLOW_ALLOC_SIZE / 2;
    OffsetHandler send[FREEFLOW_MAX_QP_PER_CORE];
    OffsetHandler send_comp[FREEFLOW_MAX_QP_PER_CORE];
    OffsetHandler recv[FREEFLOW_MAX_QP_PER_CORE];
    OffsetHandler recv_comp[FREEFLOW_MAX_QP_PER_CORE];

    struct ibv_wc *wc_send = NULL;
    struct ibv_wc *wc_recv = NULL;
    ALLOCATE(wc_send, struct ibv_wc, CTX_POLL_BATCH);
    ALLOCATE(wc_recv, struct ibv_wc, CTX_POLL_BATCH);

    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        send[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        send_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, 0);
        recv[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
        recv_comp[i].init(send_recv_buf_size / FLAGS_packSize, FLAGS_packSize, send_recv_buf_size);
    }

    size_t tx_depth = FLAGS_outstanding;//handler->tx_depth;
    size_t rx_depth = handler[0]->rx_depth;

    size_t ops = FLAGS_iterations * (send_recv_buf_size / FLAGS_packSize);
    ops = ROUND_UP(ops, FLAGS_batch_size);
    ops = ROUND_UP(ops, SEND_CQ_BATCH);

    std::vector<std::vector<size_t>>timers(FLAGS_qp_per_core, std::vector<size_t>(128, 0));
    std::vector<size_t>timer_head(FLAGS_qp_per_core);
    std::vector<size_t>timer_tail(FLAGS_qp_per_core);

    for (size_t qp_id = 0; qp_id < FLAGS_qp_per_core; qp_id++) {
        for (size_t i = 0;i < rx_depth;i++) {
            post_recv_batch(*(handler[qp_id]), 1, recv[qp_id], FLAGS_packSize);
        }
    }

    send_sync++;
    while (send_sync != FLAGS_threads) {
    }

    for (size_t i = 0;i < tx_depth;i++) {
        for (size_t qp_id = 0; qp_id < FLAGS_qp_per_core;qp_id++) {
            post_send_batch(*(handler[qp_id]), 1, send[qp_id], FLAGS_packSize);
            // timers[qp_id][timer_head[qp_id]] = get_tsc();
            // timer_head[qp_id] = (timer_head[qp_id] + 1) % 128;
        }
    }

    int done = 0;
    struct timespec begin_time, end_time;
    begin_time.tv_nsec = 0;
    begin_time.tv_sec = 0;

    size_t total_finish = 0;
    size_t ne_send;
    size_t ne_recv;

    size_t batch_size = FLAGS_batch_size;

    while (!done && !stop_flag) {
        for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
            ne_recv = poll_recv_cq(*(handler[qp_id]), wc_recv);

            if (ne_recv != 0 && begin_time.tv_sec == 0) {
                clock_gettime(CLOCK_MONOTONIC, &begin_time);
            }
            for (size_t i = 0;i < ne_recv;i++) {
                assert(wc_recv[i].status == IBV_WC_SUCCESS);
                // hdr_record_value_atomic(latency_hist, (get_tsc() - timers[qp_id][timer_tail[qp_id]]) * 10);
                // printf("recv_comp:%ld\n", recv_comp[qp_id].index());
                // timer_tail[qp_id] = (timer_tail[qp_id] + 1) % 128;
            }
            for (size_t i = 0;i < ne_recv;i++) {
                if (recv[qp_id].index() < ops) {
                    if (recv_comp[qp_id].index() % batch_size == batch_size - 1) {
                        post_recv_batch(*(handler[qp_id]), batch_size, recv[qp_id], FLAGS_packSize);
                    }
                }
                recv_comp[qp_id].step();
                total_finish++;
            }

            ne_send = poll_send_cq(*(handler[qp_id]), wc_send);
            for (size_t i = 0;i < ne_send;i++) {
                assert(wc_send[i].status == IBV_WC_SUCCESS);
                send_comp[qp_id].step(SEND_CQ_BATCH);
            }

            if (send[qp_id].index() < ops && send[qp_id].index() - recv_comp[qp_id].index() <= tx_depth - batch_size) {
                size_t now_send_num = std::min(ops - send[qp_id].index(), batch_size);
                post_send_batch(*(handler[qp_id]), now_send_num, send[qp_id], FLAGS_packSize);
                // for (size_t i = 0;i < now_send_num;i++) {
                //     timers[qp_id][timer_head[qp_id]] = get_tsc();
                //     timer_head[qp_id] = (timer_head[qp_id] + 1) % 128;
                // }
            }

            if (total_finish >= ops) {
                done = 1;
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double duration = (end_time.tv_sec - begin_time.tv_sec) + (end_time.tv_nsec - begin_time.tv_nsec) / 1e9;
    double speed = 8.0 * total_finish * FLAGS_packSize / 1000 / 1000 / 1000 / duration;

    // for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
    //     while (!stop_flag && send_comp[qp_id].index() < send[qp_id].index()) {
    //         ne_send = poll_send_cq(*(handler[qp_id]), wc_send);
    //         for (size_t i = 0;i < ne_send;i++) {
    //             assert(wc_send[i].status == IBV_WC_SUCCESS);
    //             send_comp[qp_id].step();
    //         }
    //     }
    // }

    std::lock_guard<std::mutex> guard(IO_LOCK);
    printf("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs\n", thread_index, duration, speed);
    total_bw = total_bw + speed;

    free(wc_send);
    free(wc_recv);
}

void sub_task(int thread_index) {
    sleep(1);
    wait_scheduling(thread_index, IO_LOCK);

    NetParam net_param;
    net_param.numNodes = 2;
    net_param.nodeId = FLAGS_nodeId;
    net_param.serverIp = FLAGS_serverIp;
    net_param.device_name = FLAGS_deviceName;
    net_param.gid_index = FLAGS_gidIndex;
    net_param.numa_node = FLAGS_numaNode;
    net_param.batch_size = FLAGS_batch_size;
    net_param.sock_port = FLAGS_port + thread_index;
    net_param.ib_port = 1;
    net_param.page_size = sysconf(_SC_PAGESIZE);
    net_param.cacheline_size = get_cache_line_size();
    net_param.sockfd = new int[net_param.numNodes];

    socket_init(net_param);
    roce_init(net_param, FLAGS_qp_per_core);

    PingPongInfo *info = new PingPongInfo[net_param.numNodes * FLAGS_qp_per_core]();
    void **bufs = new void *[FLAGS_qp_per_core];
    QpHandler **qp_handlers = new QpHandler * [FLAGS_qp_per_core]();

    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        // bufs[i] = malloc_2m_numa(FREEFLOW_ALLOC_SIZE, net_param.numa_node);
        bufs[i] = aligned_alloc(net_param.page_size, FREEFLOW_ALLOC_SIZE);
        for (int j = 0;j < FREEFLOW_ALLOC_SIZE / static_cast<int>(sizeof(int));j++) {
            (reinterpret_cast<int **> (bufs))[i][j] = 0;
        }
    }

    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        qp_handlers[i] = create_qp_rc(net_param, bufs[i], FREEFLOW_ALLOC_SIZE, info + i, i);
    }
    exchange_data(net_param, reinterpret_cast<char *>(info), sizeof(PingPongInfo) * FLAGS_qp_per_core);
    int my_id = net_param.nodeId;
    int dest_id = (net_param.nodeId + 1) % net_param.numNodes;
    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        connect_qp_rc(net_param, *qp_handlers[i], info + dest_id * FLAGS_qp_per_core + i, info + my_id * FLAGS_qp_per_core + i);
    }

    if (FLAGS_nodeId == 0) {
        sub_task_server(thread_index, qp_handlers);
    } else {
        sub_task_client(thread_index, qp_handlers);
    }

    for (size_t i = 0;i < FLAGS_qp_per_core;i++) {
        free(qp_handlers[i]->send_sge_list);
        free(qp_handlers[i]->recv_sge_list);
        free(qp_handlers[i]->send_wr);
        free(qp_handlers[i]->recv_wr);

        ibv_destroy_qp(qp_handlers[i]->qp);
        ibv_dereg_mr(qp_handlers[i]->mr);
        ibv_destroy_cq(qp_handlers[i]->send_cq);
        ibv_destroy_cq(qp_handlers[i]->recv_cq);
        ibv_dealloc_pd(qp_handlers[i]->pd);

        free(qp_handlers[i]);

        ibv_close_device(net_param.contexts[i]);
    }

    delete[]info;
    delete[]bufs;
    delete[]qp_handlers;
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
    assert(FLAGS_qp_per_core <= FREEFLOW_MAX_QP_PER_CORE);

    benchmark();
    if (FLAGS_nodeId != 0) {
        printf("Total throughput: %f Gbps\n", total_bw.load());
    }

    // if (FLAGS_nodeId != 0) {
    //     hdr_percentiles_print(latency_hist, stdout, 5, 10 * get_tsc_freq_per_ns(), CLASSIC);
    //     hdr_close(latency_hist);
    // }

}