#ifndef __CORE_HPP__
#define __CORE_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h> // open function
#include <unistd.h> // close function
#include <malloc.h>
#include <signal.h>
#include <infiniband/verbs.h>
#include "util.hpp"

#define BASIC_ADDR_FMT " %s address: LID %#04x QPN %#06x PSN %#06x"
#define READ_FMT       " OUT %#04x"
#define RDMA_FMT       " RKey %#08x VAddr %#016llx"
#define PERF_GID_FMT " %s: %02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d\n"

#define RESULT_LINE "---------------------------------------------------------------------------------------\n"

#define MIN_RNR_TIMER		(12)

#define DEF_QP_TIME   (14)

#define CTX_POLL_BATCH		(16)

#define KEY_MSG_SIZE_GID (108)

#define MAX_SEND_SGE		(1)
#define MAX_RECV_SGE		(1)

#define MAX(size,page_size) ((size < page_size) ? (page_size) : (size))

#define ROUND_UP(value, alignment) (((value) % (alignment) == 0) ?  \
		(value) : ((alignment) * ((value) / (alignment) + 1)))

#define UD_ADDITION         (40)

#define UD_EXTRA(type,size) ((type == UD) ? (size + UD_ADDITION) : (size))

#define RC  (0)
#define UC  (1)
#define UD  (2)

#define ALLOCATE(var,type,size)                                     \
{ if((var = (type*)malloc(sizeof(type)*(size))) == NULL)        \
	{ fprintf(stderr," Cannot Allocate\n"); exit(1);}}

const char* str_transport_type(enum ibv_transport_type t);

const char *link_layer_str(int8_t link_layer);


typedef enum {SERVER , CLIENT} MachineType;
typedef enum { SEND , WRITE, READ, ATOMIC } VerbType;
typedef enum { FA , CAS } AtomicType;

const char *verb_str(VerbType verb);

struct pingpong_dest{
	int					lid;
	int 				qpn;
	int 				psn;
	unsigned			rkey;
	unsigned long long 	vaddr;
	union ibv_gid		gid;
	int					gid_index;
	int					out_reads;
};

struct pingpong_context{
	struct ibv_context			*context;
	struct ibv_comp_channel		*channel;
	struct ibv_pd				*pd;
	struct ibv_mr				*mr;
	struct ibv_cq				*send_cq;
	struct ibv_cq				*recv_cq;
	struct ibv_ah				*ah;
	struct ibv_qp				*qp;
	struct ibv_sge				*sge_list;
	struct ibv_sge				*recv_sge_list;
	struct ibv_send_wr			*wr;
	struct ibv_recv_wr			*rwr;
	void						*buf;
	int							cache_line_size;
	int							page_size;

	uint64_t					buff_size;
};

struct perftest_parameters{
	MachineType					machine;
	uint8_t						ib_port;
	int							gid_index;
	int							out_reads;
	int 						inline_size;
	int             			pkey_index;
	char						*servername;
	int							port;
	int							sockfd;
	enum ibv_mtu				curr_mtu;
	uint64_t					iters;
	int							connection_type;
	int 						post_list;
	int 						recv_post_list;
	uint64_t					size;
	int							use_event;
	int							tx_depth;
	int 						rx_depth;
	int							eq_num;
	VerbType					verb;
	AtomicType					atomic_type;
	int							has_imm;
};

struct ibv_device* ctx_find_dev(char const *ib_devname);

struct ibv_context* ctx_open_device(struct ibv_device *ib_dev);

void check_link(struct ibv_context *context,struct perftest_parameters *user_param);

enum ibv_mtu set_mtu(struct ibv_context *context,uint8_t ib_port,int user_mtu);

int get_cache_line_size();

void ctx_wait_event(struct ibv_comp_channel *channel);

void roce_init(UserParam &user_param);

QpHandler* create_qp_rc(UserParam& user_param, void* buf, size_t size, struct PingPongInfo *info);

void connect_qp_rc(UserParam &user_param, QpHandler &qp_handler, struct PingPongInfo *info, struct PingPongInfo *my_info);

void print_pingpong_info(struct PingPongInfo *info);

void post_send(QpHandler& qp_handler, size_t offset, int length);

void post_recv(QpHandler& qp_handler, size_t offset, int length);

int poll_send_cq(QpHandler& qp_handler,struct ibv_wc *wc);

int poll_recv_cq(QpHandler& qp_handler,struct ibv_wc *wc);

#endif