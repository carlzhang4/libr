#pragma once

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
#include "minipcm.h"


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
std::atomic<bool> stop_flag = false;
// !!!!!!!
// amax2 client amax3 server
// unsigned char client_mac[6] = { 0x98,0x03,0x9b,0xca,0x48,0x38 };
// unsigned char server_mac[6] = { 0x98,0x03,0x9b,0xc7,0xc8,0x18 };

// pcie5.0-up client and down server
unsigned char client_mac[6] = { 0xa0, 0x88, 0xc2, 0x31, 0xf7, 0xde };
unsigned char server_mac[6] = { 0xa0, 0x88, 0xc2, 0x32, 0x04, 0x30 };
// !!!!!!
void ctrl_c_handler(int) { stop_flag = true; }
hdr_histogram *latency_hist = nullptr;
double scale_value = 10;
const size_t record_interval = 500000000;
std::mutex IO_LOCK;
std::atomic<int> send_sync = 0;
std::atomic<double> total_bw = 0;

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
public:
    int max_num;
    int step_size;
    int buf_offset;
    size_t cur;

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
    return 2.5;
}

static size_t get_tsc() {
    return __rdtsc();
}

void krcore_post_send(int krcore_fd, size_t offset, int qp_id, size_t batch_size, int length) {
    struct KRCORE_IOC_POST_SEND_PARAMS post_send_params;
    post_send_params.qp_id = qp_id;
    post_send_params.offset = offset;
    post_send_params.batch_size = batch_size;
    post_send_params.length = length;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POST_SEND, &post_send_params);
    if (retcode != 0 || post_send_params.success_send_cnt != batch_size) {
        printf("qp_id = %d ioctl KRCORE_IOC_POST_SEND failed\n", qp_id);
        exit(1);
    }
}

void krcore_post_recv(int krcore_fd, size_t offset, int qp_id, size_t batch_size, int length) {
    struct KRCORE_IOC_POST_RECV_PARAMS post_recv_params;
    post_recv_params.qp_id = qp_id;
    post_recv_params.offset = offset;
    post_recv_params.batch_size = batch_size;
    post_recv_params.length = length;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POST_RECV, &post_recv_params);
    if (retcode != 0 || post_recv_params.success_post_cnt != batch_size) {
        printf("qp_id = %d ioctl KRCORE_IOC_POST_RECV failed\n", qp_id);
        exit(1);
    }
}

void krcore_poll_send_cq(int krcore_fd, int max_poll_num, int qp_id, size_t &actual_poll_num) {
    struct KRCORE_IOC_POLL_SEND_CQ_PARAMS poll_send_cq_params;
    poll_send_cq_params.qp_id = qp_id;
    poll_send_cq_params.max_poll_num = max_poll_num;
    poll_send_cq_params.actual_poll_num = 0;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POLL_SEND_CQ, &poll_send_cq_params);
    if (retcode != 0) {
        printf("qp_id = %d ioctl KRCORE_IOC_POLL_SEND_CQ failed\n", qp_id);
        exit(1);
    }
    actual_poll_num = poll_send_cq_params.actual_poll_num;
}

void krcore_poll_recv_cq(int krcore_fd, int max_poll_num, int qp_id, size_t &actual_poll_num) {
    struct KRCORE_IOC_POLL_RECV_CQ_PARAMS poll_recv_cq_params;
    poll_recv_cq_params.qp_id = qp_id;
    poll_recv_cq_params.max_poll_num = max_poll_num;
    poll_recv_cq_params.actual_poll_num = 0;
    int retcode = ioctl(krcore_fd, KRCORE_IOC_POLL_RECV_CQ, &poll_recv_cq_params);
    if (retcode != 0) {
        printf("qp_id = %d ioctl KRCORE_IOC_POLL_RECV_CQ failed\n", qp_id);
        exit(1);
    }
    actual_poll_num = poll_recv_cq_params.actual_poll_num;
}

void sub_task_latency_server(int thread_index, int krcore_fd, void **user_local_buf) {
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

    PcmMemory *pcmMemory = new SPRPcmMemory(FLAGS_numaNode, get_tsc_freq_per_ns());
    // sync with other server threads
    send_sync++;
    while (send_sync != FLAGS_threads) {
    }

    pcmMemory->startRecord();
    uint32_t dummy = 0;
    size_t prev_tsc = __rdtscp(&dummy);
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

            size_t now_tsc = __rdtscp(&dummy);
            if (now_tsc - prev_tsc > get_tsc_freq_per_ns() * record_interval) {
                auto [read_bw_mb, write_bw_mb] = pcmMemory->endRecord();
                printf("%-8.0lf %-8.0lf\n", read_bw_mb, write_bw_mb);
                pcmMemory->startRecord();
                prev_tsc = __rdtscp(&dummy);
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
    delete pcmMemory;
}

void sub_task_latency_client(int thread_index, int krcore_fd, void **user_local_buf) {
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
            timers[qp_id][timer_head[qp_id]] = get_tsc();
            timer_head[qp_id] = (timer_head[qp_id] + 1) % 128;
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

    uint32_t dummy = 0;
    size_t prev_tsc = __rdtscp(&dummy);
    size_t prev_ops = 0;

    while (!done && !stop_flag) {
        for (size_t qp_id = 0;qp_id < FLAGS_qp_per_core;qp_id++) {
            krcore_poll_recv_cq(krcore_fd, KRCORE_CQ_POLL_BATCH, qp_id, actual_poll_recv_num);
            if (actual_poll_recv_num != 0 && begin_time.tv_sec == 0) {
                clock_gettime(CLOCK_MONOTONIC, &begin_time);
            }
            for (size_t i = 0;i < actual_poll_recv_num;i++) {
                hdr_record_value(latency_hist, (get_tsc() - timers[qp_id][timer_tail[qp_id]]) * scale_value);
                // printf("recv_comp:%ld\n", recv_comp[qp_id].index());
                timer_tail[qp_id] = (timer_tail[qp_id] + 1) % 128;
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
                for (size_t i = 0;i < now_send_num;i++) {
                    timers[qp_id][timer_head[qp_id]] = get_tsc();
                    timer_head[qp_id] = (timer_head[qp_id] + 1) % 128;
                }
            }

            if (total_finish >= ops) {
                done = 1;
            }
            size_t now_tsc = __rdtscp(&dummy);
            if (now_tsc - prev_tsc > get_tsc_freq_per_ns() * record_interval) {
                double avg_latency = hdr_mean(latency_hist) / (get_tsc_freq_per_ns() * scale_value * 1000);
                double p99_latency = hdr_value_at_percentile(latency_hist, 99) / (get_tsc_freq_per_ns() * scale_value * 1000);
                double p999_latency = hdr_value_at_percentile(latency_hist, 99.9) / (get_tsc_freq_per_ns() * scale_value * 1000);
                double now_speed = 8.0 * (total_finish - prev_ops) * FLAGS_packSize / record_interval;
                printf("%-6.2lf %-6.2lf %-6.2lf %-6.2lf\n", now_speed, avg_latency, p99_latency, p999_latency);
                hdr_reset(latency_hist);

                prev_tsc = __rdtscp(&dummy);
                prev_ops = total_finish;
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