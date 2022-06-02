#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <stdio.h>
#include <string.h>

using namespace std;

#define SUCCESS 0
#define FAILURE 1

class UserParam{
	public:
		int nodeId;
		string serverIp;
		int totalProcess;
};
// #define DEBUG
// #define INFO

// #define __FILENAME__ (strrchr(__FILE__, '/') + 1)

// #ifdef DEBUG
// #define LOG_D(format, ...) printf("[%s][%s][%d]: "format "\n", __FILENAME__, __FUNCTION__, __LINE__, ##VA_ARGS)
// #else
// #define LOG_D(format, ...)
// #endif

// #ifdef INFO
// #define LOG_I(format, ...) printf("[%s][%s][%d]: "format "\n", __FILENAME__, __FUNCTION__, __LINE__, ##VA_ARGS)
// #else
// #define LOG_I(format, ...)
// #endif

#endif