#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <stdio.h>
#include <string>
#include <infiniband/verbs.h>

using namespace std;

#define SUCCESS 0
#define FAILURE 1

class UserParam{
	public:
		int nodeId;
		string serverIp;
		int numNodes;
		int* sockfd;

		//system param, not changable
		uint8_t						ib_port;
		int							gid_index;
		enum ibv_mtu				cur_mtu;
		int							page_size;
		int							cacheline_size;
		struct ibv_context			*context;
};

struct PingPongInfo{
	int					lid;
	int 				qpn;
	int 				psn;
	unsigned			rkey;
	unsigned long long 	vaddr;
	union ibv_gid		gid;
	int					gid_index;
	int					out_reads;
};

class QpHandler{
	public:
		struct ibv_cq* send_cq;
		struct ibv_cq* recv_cq;
		struct ibv_qp* qp;
		struct ibv_pd* pd;
		struct ibv_mr* mr;
		struct ibv_sge* send_sge_list;
		struct ibv_sge* recv_sge_list;
		struct ibv_send_wr* send_wr;
		struct ibv_recv_wr* recv_wr;
		size_t buf;
		int max_inline_size;
		int num_wrs;
		int num_sges;
		int tx_depth;
		int rx_depth;
		int cur_tx_outstanding;
		int cur_rx_outstanding;
};

#define ALLOCATE(var,type,size)                                     \
{ if((var = (type*)malloc(sizeof(type)*(size))) == NULL)        \
	{ fprintf(stderr," Cannot Allocate\n"); exit(1);}}

#define DEBUG
#define INFO

#define __FILENAME__ (strrchr(__FILE__, '/') + 1)

#ifdef DEBUG
#define LOG_D(format, ...) \
{char buf[60];\
snprintf(buf,60,"[DEBUG][%s:%d][%s]", __FILENAME__,  __LINE__, __FUNCTION__ );\
printf("%-60s" format "\n",buf, ##__VA_ARGS__);}
#else
#define LOG_D(format, ...)
#endif

#ifdef INFO
#define LOG_I(format, ...)  \
{char buf[60];\
snprintf(buf,60,"[INFO][%s:%d][%s]", __FILENAME__,  __LINE__, __FUNCTION__ );\
printf("%-60s" format "\n",buf, ##__VA_ARGS__);}
#else
#define LOG_I(format, ...)
#endif

#define LOG_E(format, ...) fprintf(stderr, "[ERROR][%s:%d][%s]: "#format "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
exit(1);

#endif