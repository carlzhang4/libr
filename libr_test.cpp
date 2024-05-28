
#include <iostream>
#include <thread>
#include <mutex>
#include <cstdio>
#include <stdio.h>
#include <time.h>
#include "libr.hpp"

using namespace std;
std::mutex IO_LOCK;

void sub_task(int thread_index, int node_id, int is_same_addr, size_t pack_size, size_t ops, QpHandler *handler, void *buf, size_t buf_size) {
	int ne;
	struct ibv_wc *wc = NULL;
	ALLOCATE(wc, struct ibv_wc, CTX_POLL_BATCH);
	struct timespec start_timer, end_timer;
	int start_timer_flag = 1;

	if (node_id == 0) {
		size_t cur_send = 0;
		size_t cur_complete = 0;
		for (int i = 0;i < handler->tx_depth;i++) {
			if (is_same_addr) {
				post_send(*handler, 0, pack_size);
			} else {
				post_send(*handler, (cur_send * pack_size) % buf_size, pack_size);
			}
			cur_send += 1;
		}
		while (cur_complete < ops) {
			ne = poll_send_cq(*handler, wc);
			if (start_timer_flag == 1 && ne != 0) {
				clock_gettime(CLOCK_MONOTONIC, &start_timer);
				start_timer_flag = 0;
			}
			for (int i = 0;i < ne;i++) {
				if (cur_send < ops) {
					if (is_same_addr) {
						post_send(*handler, 0, pack_size);
					} else {
						post_send(*handler, (cur_send * pack_size) % buf_size, pack_size);
					}
					cur_send += 1;
				}
				assert(wc[i].status == IBV_WC_SUCCESS);
			}
			cur_complete += ne;
		}
		clock_gettime(CLOCK_MONOTONIC, &end_timer);
	} else {
		size_t cur_recv = 0;
		size_t cur_complete = 0;
		for (int i = 0;i < handler->rx_depth;i++) {
			if (is_same_addr) {
				post_recv(*handler, 0, pack_size);
			} else {
				post_recv(*handler, (cur_recv * pack_size) % buf_size, pack_size);
			}
			cur_recv += 1;
		}
		while (cur_complete < ops) {
			ne = poll_recv_cq(*handler, wc);
			if (start_timer_flag == 1 && ne != 0) {
				clock_gettime(CLOCK_MONOTONIC, &start_timer);
				start_timer_flag = 0;
			}
			for (int i = 0;i < ne;i++) {
				if (cur_recv < ops) {
					if (is_same_addr) {
						post_recv(*handler, 0, pack_size);
					} else {
						post_recv(*handler, (cur_recv * pack_size) % buf_size, pack_size);
					}
					cur_recv += 1;
				}
				assert(wc[i].status == IBV_WC_SUCCESS);
			}
			cur_complete += ne;
		}
		clock_gettime(CLOCK_MONOTONIC, &end_timer);

		size_t verify_data_size = is_same_addr ? pack_size : (ops * pack_size) % buf_size;
		for (int i = 0;i < static_cast<int>(verify_data_size / sizeof(int));i++) {
			if ((static_cast<int *> (buf))[i] != i) {
				std::lock_guard<std::mutex> guard(IO_LOCK);
				LOG_D("Data verification failed, index:%d data:%d expected_data:%d", i, (reinterpret_cast<int *>(buf))[i], i);
				break;
			}
		}
		std::lock_guard<std::mutex> guard(IO_LOCK);
		LOG_I("Data verification success!");
	}
	double duration = end_timer.tv_sec - start_timer.tv_sec + 1.0 * (end_timer.tv_nsec - start_timer.tv_nsec) / 1e9;
	std::lock_guard<std::mutex> guard(IO_LOCK);
	LOG_I("Thread : %d, running on CPU : %d", thread_index, sched_getcpu());
	LOG_I("Time : %.3f s", duration);
	LOG_I("Speed : %.2f Gb/s", 8.0 * ops * pack_size / 1024 / 1024 / 1024 / duration);
}
void rc_send_recv_benchmark_multi_qps(NetParam &net_param, int is_same_addr, size_t ops, size_t pack_size, int num_threads) {
	int num_cpus = thread::hardware_concurrency();
	LOG_I("%-20s : %d", "HardwareConcurrency", num_cpus);
	assert(num_threads <= num_cpus);
	size_t buf_size = 128 * 1024 * 1024;
	PingPongInfo *info = new PingPongInfo[net_param.numNodes * num_threads]();
	void **bufs = new void *[num_threads];
	QpHandler **qp_handlers = new QpHandler * [num_threads]();
	for (int i = 0;i < num_threads;i++) {
		// bufs[i] = memalign(net_param.page_size,buf_size);
		bufs[i] = malloc_2m_hugepage(buf_size);
		for (size_t j = 0;j < buf_size / sizeof(int);j++) {
			if (net_param.nodeId == 0) {
				(reinterpret_cast<int **>(bufs))[i][j] = j;
			} else {
				(reinterpret_cast<int **>(bufs))[i][j] = 0;
			}
		}
	}

	for (int i = 0;i < num_threads;i++) {
		qp_handlers[i] = create_qp_rc(net_param, bufs[i], buf_size, info + i);
	}
	exchange_data(net_param, reinterpret_cast<char *>(info), sizeof(PingPongInfo) * num_threads);
	for (int i = 0;i < net_param.numNodes * num_threads;i++) {
		print_pingpong_info(info + i);
	}
	int my_id = net_param.nodeId;
	int dest_id = (net_param.nodeId + 1) % net_param.numNodes;
	for (int i = 0;i < num_threads;i++) {
		connect_qp_rc(net_param, *qp_handlers[i], info + dest_id * num_threads + i, info + my_id * num_threads + i);
	}

	vector<thread> threads(num_threads);
	struct timespec start_timer, end_timer;
	clock_gettime(CLOCK_MONOTONIC, &start_timer);
	for (int i = 0;i < num_threads;i++) {
		threads[i] = thread(sub_task, i, net_param.nodeId, is_same_addr, pack_size, ops, qp_handlers[i], bufs[i], buf_size);
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(i, &cpuset);
		int rc = pthread_setaffinity_np(threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);
		if (rc != 0) {
			std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
		}
	}
	for (int i = 0;i < num_threads;i++) {
		threads[i].join();
	}
	clock_gettime(CLOCK_MONOTONIC, &end_timer);
	double duration = end_timer.tv_sec - start_timer.tv_sec + 1.0 * (end_timer.tv_nsec - start_timer.tv_nsec) / 1e9;
	LOG_I("Total Time : %.3f s", duration);
	LOG_I("Total Speed : %.2f Gb/s", 8.0 * num_threads * ops * pack_size / 1024 / 1024 / 1024 / duration);
}
void rc_send_recv_benchmark_single_qp(NetParam &net_param, int is_same_addr, int pack_size) {
	assert(net_param.numNodes == 2);
	size_t ops = 64 * 1024;
	size_t buf_size = 1 * 1024 * 1024 * 1024;
	if (!is_same_addr) {
		assert(ops * pack_size <= buf_size);
	}

	PingPongInfo *info = new PingPongInfo[net_param.numNodes]();

	void *buf = memalign(net_param.page_size, buf_size);
	QpHandler *handler = create_qp_rc(net_param, buf, buf_size, info);
	exchange_data(net_param, reinterpret_cast<char *>(info), sizeof(PingPongInfo));
	for (int i = 0;i < net_param.numNodes;i++) {
		print_pingpong_info(info + i);
	}
	for (int i = 0;i < static_cast<int>(buf_size / sizeof(int));i++) {
		if (net_param.nodeId == 0) {
			(static_cast<int *>(buf))[i] = i;
		} else {
			(static_cast<int *>(buf))[i] = 0;
		}
	}
	int my_id = net_param.nodeId;
	int dest_id = (net_param.nodeId + 1) % net_param.numNodes;
	connect_qp_rc(net_param, *handler, info + dest_id, info + my_id);

	int ne;
	struct ibv_wc *wc = NULL;
	ALLOCATE(wc, struct ibv_wc, CTX_POLL_BATCH);
	struct timespec start_timer, end_timer;
	int start_timer_flag = 1;

	if (net_param.nodeId == 0) {
		size_t cur_send = 0;
		size_t cur_complete = 0;
		for (int i = 0;i < handler->tx_depth;i++) {
			if (is_same_addr) {
				post_send(*handler, 0, pack_size);
			} else {
				post_send(*handler, cur_send * pack_size, pack_size);
			}
			cur_send += 1;
		}
		while (cur_complete < ops) {
			ne = poll_send_cq(*handler, wc);
			if (start_timer_flag == 1 && ne != 0) {
				clock_gettime(CLOCK_MONOTONIC, &start_timer);
				start_timer_flag = 0;
			}
			for (int i = 0;i < ne;i++) {
				if (cur_send < ops) {
					if (is_same_addr) {
						post_send(*handler, 0, pack_size);
					} else {
						post_send(*handler, cur_send * pack_size, pack_size);
					}
					cur_send += 1;
				}
				assert(wc[i].status == IBV_WC_SUCCESS);
			}
			cur_complete += ne;
		}
		clock_gettime(CLOCK_MONOTONIC, &end_timer);
	} else {
		size_t cur_recv = 0;
		size_t cur_complete = 0;
		for (int i = 0;i < handler->rx_depth;i++) {
			if (is_same_addr) {
				post_recv(*handler, 0, pack_size);
			} else {
				post_recv(*handler, cur_recv * pack_size, pack_size);
			}
			cur_recv += 1;
		}
		while (cur_complete < ops) {
			ne = poll_recv_cq(*handler, wc);
			if (start_timer_flag == 1 && ne != 0) {
				clock_gettime(CLOCK_MONOTONIC, &start_timer);
				start_timer_flag = 0;
			}
			for (int i = 0;i < ne;i++) {
				if (cur_recv < ops) {
					if (is_same_addr) {
						post_recv(*handler, 0, pack_size);
					} else {
						post_recv(*handler, cur_recv * pack_size, pack_size);
					}
					cur_recv += 1;
				}
				assert(wc[i].status == IBV_WC_SUCCESS);
			}
			cur_complete += ne;
		}
		clock_gettime(CLOCK_MONOTONIC, &end_timer);

		size_t verify_data_size = is_same_addr ? pack_size : ops * pack_size;
		for (int i = 0;i < static_cast<int>(verify_data_size / sizeof(int));i++) {
			if ((reinterpret_cast<int *> (buf))[i] != i) {
				LOG_D("Data verification failed, index:%d data:%d expected_data:%d", i, (reinterpret_cast<int *>(buf))[i], i);
				break;
			}
		}
		LOG_I("Data verification success!");
	}
	double duration = end_timer.tv_sec - start_timer.tv_sec + 1.0 * (end_timer.tv_nsec - start_timer.tv_nsec) / 1e9;
	LOG_I("Time : %.3f s", duration);
	LOG_I("Speed : %.2f Gb/s", 8.0 * ops * pack_size / 1024 / 1024 / 1024 / duration);
}
/*
@rc_send_recv_test
node 0 send 4 times 64 bytes data to node 1's four different places
*/
void rc_send_recv_test(NetParam &net_param, int num_qps) {
	PingPongInfo *info = new PingPongInfo[net_param.numNodes * num_qps]();
	PingPongInfo **infos = new PingPongInfo * [net_param.numNodes];
	for (int i = 0;i < net_param.numNodes;i++) {
		infos[i] = &info[i * num_qps];
	}

	size_t buf_size = 1 * 1024 * 1024;
	void *buf = memalign(net_param.page_size, buf_size);

	QpHandler **handler = new QpHandler * [num_qps]();
	for (int i = 0;i < num_qps;i++) {
		handler[i] = create_qp_rc(net_param, buf, buf_size, infos[0] + i);
	}
	exchange_data(net_param, reinterpret_cast<char *> (info), sizeof(PingPongInfo) * num_qps);
	for (int i = 0;i < net_param.numNodes * num_qps;i++) {
		print_pingpong_info(info + i);
	}

	if (net_param.numNodes == 2) {//send/recv test
		for (int i = 0;i < static_cast<int>(buf_size / sizeof(int));i++) {
			if (net_param.nodeId == 0) {
				(static_cast<int *>(buf))[i] = i;
			} else {
				(static_cast<int *>(buf))[i] = 0;
			}
		}

		int my_id = net_param.nodeId;
		int dest_id = (net_param.nodeId + 1) % net_param.numNodes;
		for (int i = 0;i < num_qps;i++) {
			connect_qp_rc(net_param, *handler[i], infos[dest_id] + i, infos[my_id] + i);
		}


		int ne;
		struct ibv_wc *wc = NULL;
		ALLOCATE(wc, struct ibv_wc, CTX_POLL_BATCH);
		int ops = 4;
		int count;
		assert(ops * num_qps * 64ul <= buf_size);
		if (net_param.nodeId == 0) {
			for (int i = 0;i < num_qps;i++) {
				for (int j = 0;j < ops;j++) {
					post_send(*handler[i], (i * ops + j) * 64, 64);
				}
			}
			for (int i = 0;i < num_qps;i++) {
				count = ops;
				do {
					ne = poll_send_cq(*handler[i], wc);
					for (int i = 0;i < ne;i++) {
						LOG_D("WC_Status:%d", static_cast<int>(wc[i].status));
						assert(wc[i].status == IBV_WC_SUCCESS);
						LOG_D("WC_ID:%d", static_cast<int>(wc[i].wr_id));
					}
					count -= ne;
				} while (count > 0);
			}
		} else {
			for (int i = 0;i < num_qps;i++) {
				for (int j = 0;j < ops;j++) {
					post_recv(*handler[i], (i * ops + j) * 64, 64);
				}
			}
			for (int i = 0;i < num_qps;i++) {
				count = ops;
				do {
					ne = poll_recv_cq(*handler[i], wc);
					for (int i = 0;i < ne;i++) {
						assert(wc[i].status == IBV_WC_SUCCESS);
						LOG_D("WC_ID:%d", static_cast<int>(wc[i].wr_id));
					}
					count -= ne;
				} while (count > 0);
			}
		}
		for (size_t i = 0;i < (ops * num_qps * 2 * 64) / sizeof(int);i++) {
			cout << (static_cast<int *>(buf))[i] << " ";
			if ((i + 1) % 16 == 0) {
				cout << endl;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	int num_threads = 48;
	NetParam net_param;
	get_opt(net_param, argc, argv);
	socket_init(net_param);
	roce_init(net_param, num_threads);
	// rc_send_recv_test(net_param,4);
	// rc_send_recv_benchmark_single_qp(net_param,0,2048);
	rc_send_recv_benchmark_multi_qps(net_param, 0, 16 * 1024 * 1024, 64, num_threads);
	return 0;
}