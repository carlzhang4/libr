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
#include "connection_manager.hpp"

#define MIN_RNR_TIMER		(12)

#define DEF_QP_TIME   (14)

#define CTX_POLL_BATCH		(16)

#define INFO_FMT "LID %#04x QPN %#06x PSN %#08x RKey %#08x VAddr %#016llx  %s: %02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d:%02d"

const char *transport_type_str(enum ibv_transport_type t);

const char *link_layer_str(int8_t link_layer);

int get_cache_line_size();

void init_net_param(NetParam &net_param);

void get_opt(NetParam &net_param, int argc, char *argv[]);

void roce_init(NetParam &net_param, int num_contexts = 1);

QpHandler *create_qp_rc(NetParam &net_param, void *buf, size_t size, struct PingPongInfo *info);

void connect_qp_rc(NetParam &net_param, QpHandler &qp_handler, struct PingPongInfo *info, struct PingPongInfo *my_info);

void print_pingpong_info(struct PingPongInfo *info);

void post_send(QpHandler &qp_handler, size_t offset, int length);

void post_send_batch(QpHandler &qp_handler, int batch_size, size_t offset, int length);

void post_recv(QpHandler &qp_handler, size_t offset, int length);

int poll_send_cq(QpHandler &qp_handler, struct ibv_wc *wc);

int poll_recv_cq(QpHandler &qp_handler, struct ibv_wc *wc);

#endif