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

#define SERVER_PORT             "6666"

void sync(void* sendData, void** receivedData, uint64_t sendDataLength, std::string serverIp, uint32_t clientNumber) noexcept;
void syncServer(uint64_t receivedDataLength, uint32_t clientNumber) noexcept;
void socket_init(UserParam &user_param);
#endif