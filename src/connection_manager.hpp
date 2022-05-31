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
#define HOST_NAME_LENGTH            256
#define ADDRESS_BUFFER_LENGTH       100

#define SERVER_IP               "127.0.0.1"
#define SERVER_PORT             "6666"
#define CLIENT_NUMBER           4

void sync(void* sendData, void** receivedData, uint64_t sendDataLength);
void syncServer(uint64_t receivedDataLength);
#endif