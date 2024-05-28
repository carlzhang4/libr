#include "util.hpp"

void *malloc_2m_hugepage(size_t size) {
	int flags = MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB | ((21 & MAP_HUGE_MASK) << MAP_HUGE_SHIFT); // 2^21 == 2M
	int protection = PROT_READ | PROT_WRITE;
	void *p = mmap(NULL, size, protection, flags, -1, 0);
	if (p == MAP_FAILED) {
		LOG_E("MAP_FAILED");
	}
	return p;
}

void *malloc_2m_numa(size_t buf_size, int node_id) {
	int page_size = 2 * 1024 * 1024;
	int num_pages = buf_size / page_size;
	void *buf = malloc_2m_hugepage(buf_size);
	for (size_t i = 0; i < buf_size / sizeof(int);i++) {
		(static_cast<int *>(buf))[i] = 0;
	}
	int *status = new int[num_pages];
	int *nodes = new int[num_pages];
	void **bufs = new void *[num_pages];
	for (int i = 0;i < num_pages;i++) {
		status[i] = 0;
		nodes[i] = node_id;
		bufs[i] = reinterpret_cast<void *>((reinterpret_cast<uint64_t>(buf) + static_cast<uint64_t>(i * page_size)));
	}
	int rc = move_pages(getpid(), num_pages, bufs, nodes, status, MPOL_MF_MOVE_ALL);
	if (rc != 0) {
		LOG_E("Move page failed, maybe you forget to use 'sudo'");
	}
	for (int i = 0;i < num_pages;i++) {
		assert(status[i] == node_id);
	}

	delete[] status;
	delete[] nodes;
	delete[] bufs;

	return buf;
}

void set_cpu(thread &t, int cpu_index) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu_index, &cpuset);
	int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);
	if (rc != 0) {
		LOG_E("%-20s : %d", "Error calling pthread_setaffinity_np", rc);
	}
}

void set_cpu_with_numa(thread &t, int cpu_index, int numa_node) {
	std::vector<size_t> cpu_cores_list;
	auto num_lcores = static_cast<size_t>(numa_num_configured_cpus());

	for (size_t i = 0; i < num_lcores; i++) {
		if (numa_node == numa_node_of_cpu(static_cast<int>(i))) {
			cpu_cores_list.push_back(i);
		}
	}

	if (static_cast<size_t>(cpu_index) >= cpu_cores_list.size()) {
		LOG_E("cpu_index[%d] >= cpu_cores_list.size()[%ld]", cpu_index, cpu_cores_list.size());
	}

	set_cpu(t, static_cast<int>(cpu_cores_list[cpu_index]));
}

int get_cpu_index_with_numa(int cpu_index, int numa_node) {
	std::vector<size_t> cpu_cores_list;
	auto num_lcores = static_cast<size_t>(numa_num_configured_cpus());

	for (size_t i = 0; i < num_lcores; i++) {
		if (numa_node == numa_node_of_cpu(static_cast<int>(i))) {
			cpu_cores_list.push_back(i);
		}
	}

	if (static_cast<size_t>(cpu_index) >= cpu_cores_list.size()) {
		LOG_E("cpu_index[%d] >= cpu_cores_list.size()[%ld]", cpu_index, cpu_cores_list.size());
	}
	return static_cast<int>(cpu_cores_list[cpu_index]);
}

void wait_scheduling(int thread_index, mutex &IO_LOCK) {
	while (thread_index != sched_getcpu()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(20));//wait set affinity success
	}
	std::lock_guard<std::mutex> guard(IO_LOCK);
	LOG_I("Thread [%2d] has been moved to core [%2d]", thread_index, sched_getcpu());
}

char *time_string() {
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	struct tm *timeinfo = localtime(&ts.tv_sec);
	static char timeStr[60];
	sprintf(timeStr, "%.2d:%.2d:%.2d.%.3ld", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, ts.tv_nsec / 1000000);
	return timeStr;
}