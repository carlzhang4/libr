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

using namespace std;
std::mutex IO_LOCK;

int ITERATIONS;
int NUM_PACK;
int PACK_SIZE;
int NUM_THREADS;
int CORE_OFFSET;
int BUF_SIZE;
string DEVICE_NAME;
int GID_INDEX;
int NUMA_NODE;
int BATCH_SIZE = 1;
int OUTSTANDING = 64;
std::atomic<bool> stop_flag = false;
std::atomic<double> total_bw = 0;
void ctrl_c_handler(int) { stop_flag = true; }

hdr_histogram *latency_hist = nullptr;
double scale_value = 10;

void sub_task_server(int thread_index, QpHandler *handler, void *buf, size_t ops) {
	wait_scheduling(thread_index, IO_LOCK);
	(void)buf;
	TimeUtil global_timer;

	int ne_recv;
	struct ibv_wc *wc_recv = NULL;
	ALLOCATE(wc_recv, struct ibv_wc, CTX_POLL_BATCH);

	OffsetHandler recv(NUM_PACK, PACK_SIZE, BUF_SIZE / 2);
	OffsetHandler recv_comp(NUM_PACK, PACK_SIZE, BUF_SIZE / 2);

	size_t rx_depth = handler->rx_depth;

	for (size_t i = 0; i < min(size_t(rx_depth), ops);i++) {
		post_recv(*handler, recv.offset(), PACK_SIZE);
		recv.step();
	}
	int done = 0;

	while (!done && !stop_flag) {
		ne_recv = poll_recv_cq(*handler, wc_recv);
		if (ne_recv != 0) {
			global_timer.start_once();
		}

		for (int i = 0;i < ne_recv;i++) {
			if (recv.index() < ops) {
				post_recv(*handler, recv.offset(), PACK_SIZE);
				recv.step();
			}
			assert(wc_recv[i].status == IBV_WC_SUCCESS);
			assert(wc_recv[i].byte_len == static_cast<uint32_t>(PACK_SIZE));
			recv_comp.step();
		}

		if (recv_comp.index() >= ops) {
			done = 1;
		}
	}
	global_timer.end();
	double duration = global_timer.get_seconds();
	double speed = 8.0 * ops * PACK_SIZE / 1000 / 1000 / 1000 / duration;

	std::lock_guard<std::mutex> guard(IO_LOCK);
	total_bw = total_bw + speed;
	LOG_I("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs", thread_index, duration, speed);

	free(wc_recv);
}

void sub_task_client(int thread_index, QpHandler *handler, void *buf, size_t ops) {
	sleep(2);
	assert(latency_hist);
	wait_scheduling(thread_index, IO_LOCK);

	(void)buf;
	TimeUtil global_timer;
	std::vector<size_t>timers(128);
	size_t timer_head = 0, timer_tail = 0;
	int ne_send;
	struct ibv_wc *wc_send = NULL;
	ALLOCATE(wc_send, struct ibv_wc, CTX_POLL_BATCH);

	OffsetHandler send(NUM_PACK, PACK_SIZE, 0);
	OffsetHandler send_comp(NUM_PACK, PACK_SIZE, 0);

	size_t tx_depth = OUTSTANDING;//handler->tx_depth;
	for (size_t i = 0; i < min(tx_depth, ops);i++) {
		post_send(*handler, send.offset(), PACK_SIZE);
		timers[timer_head] = get_tsc();
		timer_head = (timer_head + 1) % 128;
		send.step();
	}
	while (send_comp.index() < ops && !stop_flag) {
		ne_send = poll_send_cq(*handler, wc_send);
		if (ne_send != 0) {
			global_timer.start_once();
		}
		for (int i = 0;i < ne_send;i++) {
			assert(wc_send[i].status == IBV_WC_SUCCESS);
			hdr_record_value_atomic(latency_hist, (get_tsc() - timers[timer_tail]) * 10);
			timer_tail = (timer_tail + 1) % 128;
			send_comp.step();
		}
		if (send.index() < ops && send.index() - send_comp.index() < tx_depth) {
			post_send(*handler, send.offset(), PACK_SIZE);
			timers[timer_head] = get_tsc();
			timer_head = (timer_head + 1) % 128;
			send.step();
		}
	}
	while (send_comp.index() < send.index()) {
		ne_send = poll_send_cq(*handler, wc_send);
		for (int i = 0;i < ne_send;i++) {
			assert(wc_send[i].status == IBV_WC_SUCCESS);
			hdr_record_value_atomic(latency_hist, (get_tsc() - timers[timer_tail]) * 10);
			timer_tail = (timer_tail + 1) % 128;
			send_comp.step();
		}
	}
	global_timer.end();
	double duration = global_timer.get_seconds();
	double speed = 8.0 * send_comp.index() * PACK_SIZE / 1000 / 1000 / 1000 / duration;

	std::lock_guard<std::mutex> guard(IO_LOCK);
	total_bw = total_bw + speed;
	LOG_I("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs", thread_index, duration, speed);

	free(wc_send);
}

void benchmark(NetParam &net_param) {
	int num_cpus = thread::hardware_concurrency();
	LOG_I("%-20s : %d", "HardwareConcurrency", num_cpus);
	assert(NUM_THREADS <= num_cpus);

	BUF_SIZE = NUM_PACK * PACK_SIZE * 2;
	size_t ops = size_t(1) * ITERATIONS * NUM_PACK;
	LOG_I("OPS : [%ld]", ops);

	PingPongInfo *info = new PingPongInfo[net_param.numNodes * NUM_THREADS]();
	void **bufs = new void *[NUM_THREADS];
	QpHandler **qp_handlers = new QpHandler * [NUM_THREADS]();
	for (int i = 0;i < NUM_THREADS;i++) {
		bufs[i] = malloc_2m_numa(BUF_SIZE, net_param.numa_node);
		for (int j = 0;j < BUF_SIZE / static_cast<int>(sizeof(int));j++) {
			(reinterpret_cast<int **> (bufs))[i][j] = 0;
		}
	}

	for (int i = 0;i < NUM_THREADS;i++) {
		qp_handlers[i] = create_qp_rc(net_param, bufs[i], BUF_SIZE, info + i, i);
	}
	exchange_data(net_param, reinterpret_cast<char *>(info), sizeof(PingPongInfo) * NUM_THREADS);
	int my_id = net_param.nodeId;
	int dest_id = (net_param.nodeId + 1) % net_param.numNodes;
	for (int i = 0;i < NUM_THREADS;i++) {
		connect_qp_rc(net_param, *qp_handlers[i], info + dest_id * NUM_THREADS + i, info + my_id * NUM_THREADS + i);
	}

	vector<thread> threads(NUM_THREADS);
	struct timespec start_timer, end_timer;
	clock_gettime(CLOCK_MONOTONIC, &start_timer);
	for (int i = 0;i < NUM_THREADS;i++) {
		int now_index = get_cpu_index_with_numa(i + CORE_OFFSET, net_param.numa_node);
		if (net_param.nodeId == 0) {
			threads[i] = thread(sub_task_server, now_index, qp_handlers[i], bufs[i], ops);
		} else if (net_param.nodeId == 1) {
			threads[i] = thread(sub_task_client, now_index, qp_handlers[i], bufs[i], ops);
		}
		set_cpu_with_numa(threads[i], i + CORE_OFFSET, net_param.numa_node);
	}
	for (int i = 0;i < NUM_THREADS;i++) {
		threads[i].join();
	}
	clock_gettime(CLOCK_MONOTONIC, &end_timer);
	printf("Total bandwidth: %f Gbps\n", total_bw.load());
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

	delete[]info;
	delete[]bufs;
	delete[]qp_handlers;
}

DEFINE_int32(iterations, 1000, "iterations");
DEFINE_int32(packSize, 4096, "packSize");
DEFINE_int32(threads, 1, "num_threads");
DEFINE_int32(nodeId, 0, "nodeId");
DEFINE_string(serverIp, "", "serverIp");
DEFINE_int32(coreOffset, 0, "coreOffset");
DEFINE_int32(numPack, 1024, "numPack");
DEFINE_string(deviceName, "mlx5_0", "deviceName");
DEFINE_int32(gidIndex, 3, "gidIndex");
DEFINE_int32(numaNode, 0, "numaNode");
DEFINE_int32(port, 6666, "bind_port");

int main(int argc, char *argv[]) {
	signal(SIGINT, ctrl_c_handler);
	signal(SIGTERM, ctrl_c_handler);

	gflags::ParseCommandLineFlags(&argc, &argv, true);

	ITERATIONS = FLAGS_iterations;
	PACK_SIZE = FLAGS_packSize;
	NUM_THREADS = FLAGS_threads;
	CORE_OFFSET = FLAGS_coreOffset;
	NUM_PACK = FLAGS_numPack;
	DEVICE_NAME = FLAGS_deviceName;
	GID_INDEX = FLAGS_gidIndex;
	NUMA_NODE = FLAGS_numaNode;

	NetParam net_param;
	net_param.numNodes = 2;
	net_param.nodeId = FLAGS_nodeId;
	net_param.serverIp = FLAGS_serverIp;
	net_param.device_name = DEVICE_NAME;
	net_param.gid_index = GID_INDEX;
	net_param.numa_node = NUMA_NODE;
	net_param.batch_size = BATCH_SIZE;
	net_param.sge_per_wr = 1;
	net_param.sock_port = FLAGS_port;
	net_param.use_devx_context = false;

	if (FLAGS_nodeId != 0) {
		hdr_init(1000, 50000000, 3, &latency_hist);
	}

	init_net_param(net_param);
	socket_init(net_param);
	roce_init(net_param, NUM_THREADS);
	benchmark(net_param);

	if (FLAGS_nodeId != 0) {
		hdr_percentiles_print(latency_hist, stdout, 5, 10 * get_tsc_freq_per_ns(), CLASSIC);
		hdr_close(latency_hist);
	}
}