#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

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
#include "core.hpp"

void socket_connect(struct perftest_parameters*	user_param);

int ctx_xchg_data_ethernet(struct perftest_parameters*  user_param, void *my_data,void *rem_data,int size);

int ctx_hand_shake(struct perftest_parameters* user_param, struct pingpong_dest *my_dest, struct pingpong_dest *rem_dest);

void ctx_print_pingpong_data(struct pingpong_dest *element, struct perftest_parameters* user_param, char const* side);
#endif