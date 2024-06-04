#ifndef __CONNECTION_MANAGER_HPP__
#define __CONNECTION_MANAGER_HPP__
#include <cstring>
#include <string>
#include <set>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <atomic>
#include <ctime>
#include "util.hpp"


void socket_init(NetParam &net_param);
void exchange_data(NetParam &net_param, char *data, int size);
#endif