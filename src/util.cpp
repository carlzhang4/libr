#include "util.hpp"

void *myMalloc2MbPage(size_t size) {
	int flags = MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB | ((21 & MAP_HUGE_MASK) << MAP_HUGE_SHIFT); // 2^21 == 2M
	int protection = PROT_READ | PROT_WRITE;
	void* p = mmap(NULL, size, protection, flags, -1, 0);
	if(p == MAP_FAILED){
		LOG_E("MAP_FAILED");
	}
	return mmap(NULL, size, protection, flags, -1, 0);
}