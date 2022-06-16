#include "util.hpp"

void *malloc_2m_hugepage(size_t size) {
	int flags = MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB | ((21 & MAP_HUGE_MASK) << MAP_HUGE_SHIFT); // 2^21 == 2M
	int protection = PROT_READ | PROT_WRITE;
	void* p = mmap(NULL, size, protection, flags, -1, 0);
	if(p == MAP_FAILED){
		LOG_E("MAP_FAILED");
	}
	return mmap(NULL, size, protection, flags, -1, 0);
}

void set_cpu(thread& t,int cpu_index){
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu_index, &cpuset);
	int rc = pthread_setaffinity_np(t.native_handle(),sizeof(cpu_set_t), &cpuset);
	if (rc != 0) {
		LOG_E("%-20s : %d","Error calling pthread_setaffinity_np",rc);
	}
}