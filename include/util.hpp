#ifndef __UTIL_LIBR_HPP__
#define __UTIL_LIBR_HPP__

#include <stdio.h>
#include <string>
#include <infiniband/verbs.h>
#include <sys/mman.h>
#include <thread>
#include <vector>
#include <assert.h>
#include <numaif.h>
#include <numa.h>
#include <unistd.h>
#include <mutex>

using namespace std;

#define SUCCESS 0
#define FAILURE 1

class NetParam {
public:
	int nodeId;
	string serverIp;
	int numNodes;
	int *sockfd;
	int sock_port;

	//system param, not changable
	string 						device_name;
	uint8_t						ib_port;
	int							gid_index;
	int 						numa_node;
	enum ibv_mtu				cur_mtu;
	int							page_size;
	int							cacheline_size;
	struct ibv_context **contexts;
	int							num_contexts;
	int 						batch_size;
};

struct PingPongInfo {
	int					lid;
	int 				qpn;
	int 				psn;
	unsigned			rkey;
	unsigned long long 	vaddr;
	union ibv_gid		gid;
	int					gid_index;
	int					out_reads;
};

class QpHandler {
public:
	struct ibv_cq *send_cq;
	struct ibv_cq *recv_cq;
	struct ibv_qp *qp;
	struct ibv_pd *pd;
	struct ibv_mr *mr;
	struct ibv_sge *send_sge_list;
	struct ibv_sge *recv_sge_list;
	struct ibv_send_wr *send_wr;
	struct ibv_recv_wr *recv_wr;
	struct ibv_send_wr *send_bar_wr;
	struct ibv_recv_wr *recv_bar_wr;
	size_t buf;
	size_t remote_buf;
	unsigned int remote_rkey;
	int max_inline_size;
	int num_wrs;
	int num_sges;
	int tx_depth;
	int rx_depth;
};

#define ALLOCATE(var,type,size)                                     \
{ if((var = static_cast<type*>(malloc(sizeof(type)*(size)))) == NULL)        \
	{ fprintf(stderr," Cannot Allocate\n"); exit(1);}}

#define DEBUG
#define INFO

#define __FILENAME__ (strrchr(__FILE__, '/') + 1)

#ifdef DEBUG
#define LOG_D(format, ...) \
{char buf[100];\
snprintf(buf,100,"[DEBUG][%s][%s:%d][%s]",time_string(), __FILENAME__,  __LINE__, __FUNCTION__ );\
printf("%-100s" format "\n",buf, ##__VA_ARGS__);}
#else
#define LOG_D(format, ...)
#endif

#ifdef INFO
#define LOG_I(format, ...)  \
{char buf[100];\
snprintf(buf,100,"[INFO][%s][%s:%d][%s]",time_string(), __FILENAME__,  __LINE__, __FUNCTION__ );\
printf("%-100s" format "\n",buf, ##__VA_ARGS__);}
#else
#define LOG_I(format, ...)
#endif

#define LOG_E(format, ...) fprintf(stderr, "[ERROR][%s][%s:%d][%s]: " format "\n",time_string(), __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
exit(1);

void *malloc_2m_hugepage(size_t size);

void *malloc_2m_numa(size_t buf_size, int node_id);

void set_cpu(thread &t, int cpu_index);

void set_cpu_with_numa(thread &t, int cpu_index, int numa_node);

int get_cpu_index_with_numa(int cpu_index, int numa_node);

void wait_scheduling(int thread_index, mutex &IO_LOCK);

char *time_string();

class TimeUtil {
private:
	int MAX_INFLIGHT;
	int STEP;//1us divide into #STEP shares
	int BUCKET_NUM;
	int execute_once_flag;
	double *start_arr;
	size_t start_idx;
	size_t end_idx;
	int *buckets;
public:
	struct timespec cur_time;
	double duration_us;
	TimeUtil(int bucket_num = 1024, int step = 10, int max_inflight = 512) {
		execute_once_flag = 1;
		duration_us = 0.0;

		BUCKET_NUM = bucket_num;
		STEP = step;
		MAX_INFLIGHT = max_inflight;
		start_arr = static_cast<double *>(malloc(max_inflight * sizeof(double)));
		buckets = static_cast<int *>(malloc(bucket_num * sizeof(int)));
		memset(buckets, 0, bucket_num * sizeof(int));
		start_idx = 0;
		end_idx = 0;
	}
	void start_once() {
		if (execute_once_flag) {
			start();
			execute_once_flag = 0;
		}
	}
	void start() {
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		double t_us = 1.0 * (cur_time.tv_sec * 1e6 + cur_time.tv_nsec / 1e3);
		duration_us -= t_us;

		start_arr[start_idx % MAX_INFLIGHT] = t_us;
		start_idx++;
	}
	void end() {
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		double t_us = 1.0 * (cur_time.tv_sec * 1e6 + cur_time.tv_nsec / 1e3);
		duration_us += t_us;

		double latency = t_us - start_arr[end_idx % MAX_INFLIGHT];
		int l = static_cast<int>(latency * STEP);
		if (l >= BUCKET_NUM) {
			buckets[BUCKET_NUM - 1]++;
		} else {
			buckets[l]++;
		}
		end_idx++;
	}
	void show(string str) {
		assert(start_idx == end_idx);
		LOG_I("%s [%.2f] us, average [%.2f] us", str.c_str(), duration_us, duration_us / start_idx);
	}
	double get_seconds() {
		assert(start_idx == end_idx);
		return duration_us / 1e6;
	}
	void show_percentage(double per, string str) {
		size_t num = 0;
		size_t div = static_cast<size_t>(per * start_idx);
		for (int i = 0;i < BUCKET_NUM;i++) {
			num += buckets[i];
			if (num >= div) {
				LOG_I("%s [%.2f] us", str.c_str(), 1.0 * i / STEP);
				break;
			}
		}
	}
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
#endif