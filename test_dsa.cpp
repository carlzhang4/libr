#include <bits/stdc++.h>
#include <gflags/gflags.h>

#include "libr.hpp"
#include <hdr/hdr_histogram.h>
#include "dml/dml.h"

std::atomic<bool> stop_flag = false;
std::atomic<double> total_bw = 0;
void ctrl_c_handler(int) { stop_flag = true; }

DEFINE_int32(payload, 4096, "payload");
DEFINE_int32(threads, 1, "threads");
DEFINE_int32(numaNode, 0, "numaNode");
DEFINE_uint64(batchSize, 32, "batchSize");
size_t BUF_SIZE = 128 * 1024 * 1024;

std::mutex IO_LOCK;

void sub_task(int thread_index, void *src_buf, void *dst_buf) {
    wait_scheduling(thread_index, IO_LOCK);
    TimeUtil global_timer;

    uint32_t size = 0u;
    dml_status_t status = dml_get_job_size(DML_PATH_HW, &size);
    dml_job_t *dml_job_ptr = NULL;

    std::vector<dml_job_t *> dml_jobs(FLAGS_batchSize, nullptr);
    for (size_t i = 0;i < FLAGS_batchSize;i++) {
        dml_jobs[i] = (dml_job_t *)malloc(size);
        dml_job_ptr = dml_jobs[i];
        status = dml_init_job(DML_PATH_HW, dml_job_ptr);
        if (status != DML_STATUS_OK) {
            printf("dml_init_job failed\n");
            exit(1);
        }
        dml_job_ptr->operation = DML_OP_MEM_MOVE;
        dml_job_ptr->numa_id = FLAGS_numaNode;
    }

    OffsetHandler submit(BUF_SIZE / FLAGS_payload, FLAGS_payload, 0);
    OffsetHandler submit_comp(BUF_SIZE / FLAGS_payload, FLAGS_payload, 0);


    global_timer.start();
    size_t ops = 10000000;
    while (submit_comp.index() < ops && !stop_flag) {
        while (submit.index() - submit_comp.index() < FLAGS_batchSize && submit.index() < ops) {
            dml_job_t *now_job = dml_jobs[submit.index() % FLAGS_batchSize];
            now_job->source_first_ptr = reinterpret_cast<uint8_t *>(reinterpret_cast<uint64_t>(src_buf) + submit.offset());
            now_job->source_length = FLAGS_payload;
            now_job->destination_first_ptr = reinterpret_cast<uint8_t *>(reinterpret_cast<uint64_t>(dst_buf) + submit.offset());
            now_job->destination_length = FLAGS_payload;
            status = dml_submit_job(now_job);
            if (status != DML_STATUS_OK) {
                printf("dml_submit_job failed\n");
                exit(1);
            }
            submit.step();
        }
        while (submit_comp.index() < submit.index()) {
            dml_job_t *now_job = dml_jobs[submit_comp.index() % FLAGS_batchSize];
            status = dml_check_job(now_job);
            if (status == DML_STATUS_OK) {
                submit_comp.step();
            } else if (status == DML_STATUS_JOB_CORRUPTED) {
                printf("dml_check_job failed\n");
                exit(1);
            } else if (status == DML_STATUS_BEING_PROCESSED) {
                break;
            } else {
                printf("unreachable\n");
                exit(1);
            }
        }
    }

    global_timer.end();
    double duration = global_timer.get_seconds();
    double speed = 8.0 * submit_comp.index() * FLAGS_payload / 1000 / 1000 / 1000 / duration;

    std::lock_guard<std::mutex> guard(IO_LOCK);
    printf("thread [%d], duration [%f]s, throughput [%f] Gpbs\n", thread_index, duration, speed);
    total_bw = total_bw + speed;
}

void benchmark() {

    void **src_bufs = new void *[FLAGS_threads];
    void **dst_bufs = new void *[FLAGS_threads];
    for (int i = 0;i < FLAGS_threads;i++) {
        src_bufs[i] = malloc_2m_numa(BUF_SIZE, FLAGS_numaNode);
        dst_bufs[i] = malloc_2m_numa(BUF_SIZE, FLAGS_numaNode);
        for (size_t j = 0;j < BUF_SIZE / sizeof(int);j++) {
            (reinterpret_cast<int **>(src_bufs))[i][j] = j;
            (reinterpret_cast<int **>(dst_bufs))[i][j] = j + 1;
        }
    }

    vector<thread> threads(FLAGS_threads);
    for (int i = 0;i < FLAGS_threads;i++) {
        int now_index = get_cpu_index_with_numa(i, FLAGS_numaNode);
        threads[i] = thread(sub_task, now_index, src_bufs[i], dst_bufs[i]);

        set_cpu_with_numa(threads[i], i, FLAGS_numaNode);
    }
    for (int i = 0;i < FLAGS_threads;i++) {
        threads[i].join();
    }
    printf("Total bandwidth: %f Gbps\n", total_bw.load());
}


int main(int argc, char *argv[]) {
    signal(SIGINT, ctrl_c_handler);
    signal(SIGTERM, ctrl_c_handler);

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    benchmark();
}