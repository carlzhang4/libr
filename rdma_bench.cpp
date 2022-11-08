#include <iostream>
#include <thread>
#include <mutex>
#include <cstdio>
#include <stdio.h>
#include <time.h>
#include <gflags/gflags.h>
#include "src/libr.hpp"

using namespace std;
std::mutex IO_LOCK;

int ITERATIONS;
int NUM_PACK;
int PACK_SIZE;
int NUM_THREADS;
int CORE_OFFSET;
int BUF_SIZE;
int OUTSTANDING = 48;

class TimeUtil{
	public:
		int execute_once_flag;
		struct timespec cur_time;
		double duration_us;
		int start_count;
		int end_count;
	TimeUtil(){
		execute_once_flag = 1;
		duration_us = 0.0;
		start_count = 0;
		end_count = 0;
	}
	void start_once(){
		if(execute_once_flag){
			start();
			execute_once_flag = 0;
		}
	}
	void start(){
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		duration_us -= 1.0*(cur_time.tv_sec*1e6 + cur_time.tv_nsec/1e3);
		start_count++;
		// LOG_D("Start seconds [%d] s",cur_time.tv_sec);
	}
	void end(){
		clock_gettime(CLOCK_MONOTONIC, &cur_time);
		duration_us += 1.0*(cur_time.tv_sec*1e6 + cur_time.tv_nsec/1e3);
		end_count++;
		// LOG_D("End seconds [%d] s",cur_time.tv_sec);
	}
	void show(string str){
		assert(start_count==end_count);
		LOG_I("%s [%.2f] us, average [%.2f] us",str.c_str(),duration_us,duration_us/start_count);
	}
	double get_seconds(){
		assert(start_count==end_count);
		return duration_us/1e6;
	}
};

class OffsetHandler{
	private:
		int max_num;
		int step_size;
		int buf_offset;
		size_t cur;
	
	public:
	OffsetHandler(){
		cur = 0;
	}

	OffsetHandler(int max_num,int step_size,int buf_offset):max_num(max_num),step_size(step_size),buf_offset(buf_offset){	
		cur = 0;
	}
	void init(int max_num,int step_size,int buf_offset){
		cur = 0;
		this->max_num = max_num;
		this->step_size = step_size;
		this->buf_offset = buf_offset;
	}
	size_t step(){
		size_t ret = offset();
		cur+=1;
		return ret;
	}
	size_t offset(){
		return (cur%max_num)*step_size + buf_offset;
	}
	size_t index(){
		return cur;
	}
	int index_mod(){
		return cur%max_num;
	}
};

void sub_task_server(int thread_index, QpHandler* handler, void* buf, size_t ops){
	wait_scheduling(thread_index,IO_LOCK);
	TimeUtil global_timer;

	int ne_send;
	int ne_recv;
	struct ibv_wc *wc_send = NULL;
	struct ibv_wc *wc_recv = NULL;
	ALLOCATE(wc_send ,struct ibv_wc ,CTX_POLL_BATCH);
	ALLOCATE(wc_recv ,struct ibv_wc ,CTX_POLL_BATCH);

	OffsetHandler send(NUM_PACK,PACK_SIZE,BUF_SIZE/2);
	OffsetHandler recv(NUM_PACK,PACK_SIZE,BUF_SIZE/2);
	OffsetHandler send_comp(NUM_PACK,PACK_SIZE,0);
	OffsetHandler recv_comp(NUM_PACK,PACK_SIZE,BUF_SIZE/2);

	int tx_depth = handler->tx_depth;
	int rx_depth = handler->rx_depth;

	for(size_t i=0; i<min(size_t(rx_depth),ops);i++){
		post_recv(*handler,recv.offset(),PACK_SIZE);
		recv.step();
	}
	int done = 0;

	while(!done){
		ne_recv = poll_recv_cq(*handler,wc_recv);
		if(ne_recv!=0){
			global_timer.start_once();
		}

		for(int i=0;i<ne_recv;i++){
			if(recv.index()<ops){
				post_recv(*handler,recv.offset(),PACK_SIZE);
				recv.step();
			}
			assert(wc_recv[i].status == IBV_WC_SUCCESS);
			assert(wc_recv[i].byte_len == PACK_SIZE);
			recv_comp.step();
		}

		while(send.index()<recv_comp.index() && (send.index()-send_comp.index())<tx_depth){
			post_send(*handler,send.offset(),PACK_SIZE);
			send.step();
		}

		ne_send = poll_send_cq(*handler,wc_send);
		for(int i=0;i<ne_send;i++){
			if(wc_send[i].status != IBV_WC_SUCCESS){
				LOG_E("Thread : %d, wc_send[%ld].status = %d, i=%d",thread_index, send_comp.index(), wc_send[i].status, i);
			}
			send_comp.step();
		}
		done = 1;
		if(recv_comp.index()<ops || send_comp.index()<ops){
			done = 0;
		}
	}
	global_timer.end();
	double duration = global_timer.get_seconds();
	double speed = 8.0*ops*PACK_SIZE/1024/1024/1024/duration;

	size_t verify_size = NUM_PACK*PACK_SIZE;
	int*recv_data = (int*)(((size_t)buf)+BUF_SIZE/2);
	for(size_t i=0;i<verify_size/sizeof(int);i++){
		if(recv_data[i] != i){
			std::lock_guard<std::mutex> guard(IO_LOCK);
			LOG_E("Data verification failed, index:%ld data:%d expected_data:%ld",i,recv_data[i],i);
			break;
		}
	}
	std::lock_guard<std::mutex> guard(IO_LOCK);
	LOG_I("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs",thread_index,duration,speed);
	
}

void sub_task_client(int thread_index, QpHandler* handler, void* buf, size_t ops){
	wait_scheduling(thread_index,IO_LOCK);
	TimeUtil global_timer;
	TimeUtil timer;

	int ne_send;
	int ne_recv;
	struct ibv_wc *wc_send = NULL;
	struct ibv_wc *wc_recv = NULL;
	ALLOCATE(wc_send ,struct ibv_wc ,CTX_POLL_BATCH);
	ALLOCATE(wc_recv ,struct ibv_wc ,CTX_POLL_BATCH);

	OffsetHandler send(NUM_PACK,PACK_SIZE,0);
	OffsetHandler recv(NUM_PACK,PACK_SIZE,BUF_SIZE/2);
	OffsetHandler send_comp(NUM_PACK,PACK_SIZE,0);
	OffsetHandler recv_comp(NUM_PACK,PACK_SIZE,BUF_SIZE/2);

	int tx_depth = OUTSTANDING;//handler->tx_depth;
	int rx_depth = handler->rx_depth;
	for(size_t i=0; i<min(size_t(tx_depth),ops);i++){
		post_send(*handler,send.offset(),PACK_SIZE);
		timer.start();
		send.step();
	}
	for(size_t i=0; i<min(size_t(rx_depth),ops);i++){
		post_recv(*handler,recv.offset(),PACK_SIZE);
		recv.step();
	}
	while(recv_comp.index()<ops || send_comp.index()<ops){
		ne_send = poll_send_cq(*handler, wc_send);
		if(ne_send!=0){
			global_timer.start_once();
		}
		for(int i=0;i<ne_send;i++){
			assert(wc_send[i].status == IBV_WC_SUCCESS);
			send_comp.step();
		}
		if(send.index()<ops && send.index()-recv_comp.index()<tx_depth){
			post_send(*handler,send.offset(),PACK_SIZE);
			timer.start();
			send.step();
		}

		ne_recv = poll_recv_cq(*handler, wc_recv);
		for(int i=0;i<ne_recv;i++){
			if(recv.index()<ops){
				post_recv(*handler,recv.offset(),PACK_SIZE);
				recv.step();
			}
			assert(wc_recv[i].status == IBV_WC_SUCCESS);
			if(wc_recv[i].byte_len != PACK_SIZE){
				LOG_E("Client thread[%d] verify length failed, index:[%ld], byte_len:[%d]",thread_index,recv_comp.index(),wc_recv[i].byte_len);
			}
			timer.end();
			recv_comp.step();
		}
	}
	global_timer.end();
	double duration = global_timer.get_seconds();
	double speed = 8.0*ops*PACK_SIZE/1024/1024/1024/duration;

	size_t verify_size = NUM_PACK*PACK_SIZE;
	int*recv_data = (int*)(((size_t)buf)+BUF_SIZE/2);
	for(size_t i=0;i<verify_size/sizeof(int);i++){
		if(recv_data[i] != i){
			std::lock_guard<std::mutex> guard(IO_LOCK);
			LOG_E("Data verification failed, index:%ld data:%d expected_data:%ld",i,recv_data[i],i);
			break;
		}
	}
	std::lock_guard<std::mutex> guard(IO_LOCK);
	LOG_I("Data verification success, thread [%d], duration [%f]s, throughput [%f] Gpbs",thread_index,duration,speed);

	timer.show("Latency");
}

void benchmark(NetParam &net_param){
	int num_cpus = thread::hardware_concurrency();
	LOG_I("%-20s : %d","HardwareConcurrency",num_cpus);
	assert(NUM_THREADS<=num_cpus);

	assert(NUM_PACK*PACK_SIZE < BUF_SIZE/2);
	size_t ops = size_t(1) * ITERATIONS * NUM_PACK;
	LOG_I("OPS : [%ld]",ops);

	PingPongInfo *info = new PingPongInfo[net_param.numNodes*NUM_THREADS]();
	void** bufs = new void*[NUM_THREADS];
	QpHandler** qp_handlers = new QpHandler*[NUM_THREADS]();
	for(int i=0;i<NUM_THREADS;i++){
		bufs[i] = malloc_2m_hugepage(BUF_SIZE);
		for(int j=0;j<BUF_SIZE/sizeof(int);j++){
			if(net_param.nodeId == 0){
				((int**)bufs)[i][j] = 0;
			}else{
				((int**)bufs)[i][j] = j;
			}
		}
	}

	for(int i=0;i<NUM_THREADS;i++){
		qp_handlers[i] = create_qp_rc(net_param,bufs[i],BUF_SIZE,info+i);
	}
	exchange_data(net_param, (char*)info, sizeof(PingPongInfo)*NUM_THREADS);
	int my_id = net_param.nodeId;
	int dest_id = (net_param.nodeId+1)%net_param.numNodes;
	for(int i=0;i<NUM_THREADS;i++){
		connect_qp_rc(net_param,*qp_handlers[i],info+dest_id*NUM_THREADS+i,info+my_id*NUM_THREADS+i);
	}

	vector<thread> threads(NUM_THREADS);
	struct timespec start_timer,end_timer;
	clock_gettime(CLOCK_MONOTONIC, &start_timer);
	for(int i=0;i<NUM_THREADS;i++){
		if(net_param.nodeId == 0){
			threads[i] = thread(sub_task_server, i+CORE_OFFSET, qp_handlers[i], bufs[i], ops);
		}else if(net_param.nodeId == 1){
			threads[i] = thread(sub_task_client, i+CORE_OFFSET, qp_handlers[i], bufs[i], ops);
		}
		set_cpu(threads[i],i+CORE_OFFSET);
	}
	for(int i=0;i<NUM_THREADS;i++){
		threads[i].join();
	}
	clock_gettime(CLOCK_MONOTONIC, &end_timer);
}

DEFINE_int32(iterations,			1000,	"iterations");
DEFINE_int32(packSize,				4096,	"packSize");
DEFINE_int32(threads,				1,		"num_threads");
DEFINE_int32(nodeId,				0,		"nodeId");
DEFINE_string(serverIp,				"",		"serverIp");
DEFINE_int32(coreOffset,			0,		"coreOffset");
DEFINE_int32(bufSize,				1073741824,	"bufSize");
DEFINE_int32(numPack,				1024,	"numPack");


int main(int argc, char* argv[]){
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	ITERATIONS	= FLAGS_iterations;
	PACK_SIZE	= FLAGS_packSize;
	NUM_THREADS	= FLAGS_threads;
	CORE_OFFSET	= FLAGS_coreOffset;
	BUF_SIZE	= FLAGS_bufSize;
	NUM_PACK	= FLAGS_numPack;


	NetParam net_param;
	net_param.numNodes = 2;
	net_param.nodeId = FLAGS_nodeId;
	net_param.serverIp = FLAGS_serverIp;
	init_net_param(net_param);

	socket_init(net_param);
    roce_init(net_param,NUM_THREADS);
	benchmark(net_param);
}