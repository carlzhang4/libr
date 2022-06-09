
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <time.h>
#include "src/core.hpp"
#include "src/util.hpp"
#include "src/connection_manager.hpp"

using namespace std;

void rc_send_recv_benchmark_single_qp(UserParam &user_param, int is_same_addr,int pack_size){
	assert(user_param.numNodes == 2);
	int ops = 64*1024;
	size_t buf_size = 1*1024*1024*1024;
	if(!is_same_addr){
		assert(ops*pack_size <= buf_size);
	}
	
	PingPongInfo *info = new PingPongInfo[user_param.numNodes]();
	
	void* buf = memalign(user_param.page_size,buf_size);
	QpHandler* handler = create_qp_rc(user_param,buf,buf_size,info);
	exchange_data(user_param, (char*)info, sizeof(PingPongInfo));
	for(int i=0;i<user_param.numNodes;i++){
		print_pingpong_info(info+i);
	}
	for(int i=0;i<buf_size/sizeof(int);i++){
		if(user_param.nodeId == 0){
			((int*)buf)[i] = i;
		}else{
			((int*)buf)[i] = 0;
		}
	}
	int my_id = user_param.nodeId;
	int dest_id = (user_param.nodeId+1)%user_param.numNodes;
	connect_qp_rc(user_param,*handler,info+dest_id,info+my_id);
	
	int ne;
	struct ibv_wc *wc = NULL;
	ALLOCATE(wc ,struct ibv_wc ,CTX_POLL_BATCH);
	struct timespec start_timer,end_timer;
	int start_timer_flag = 1;
	
	if(user_param.nodeId==0){
		int cur_send = 0;
		int cur_complete = 0;
		for(int i=0;i<handler->tx_depth;i++){
			if(is_same_addr){
				post_send(*handler,0,pack_size);
			}else{
				post_send(*handler,cur_send*pack_size,pack_size);
			}
			cur_send+=1;
		}
		while(cur_complete<ops){
			ne = poll_send_cq(*handler,wc);
			if(start_timer_flag==1 && ne!=0){
				clock_gettime(CLOCK_MONOTONIC, &start_timer);
				start_timer_flag=0;
			}
			for(int i=0;i<ne;i++){
				if(cur_send<ops){
					if(is_same_addr){
						post_send(*handler,0,pack_size);
					}else{
						post_send(*handler,cur_send*pack_size,pack_size);
					}
					cur_send+=1;
				}
				assert(wc[i].status == IBV_WC_SUCCESS);
			}
			cur_complete+=ne;
		}
		clock_gettime(CLOCK_MONOTONIC, &end_timer);
	}else{
		int cur_recv = 0;
		int cur_complete = 0;
		for(int i=0;i<handler->rx_depth;i++){
			if(is_same_addr){
				post_recv(*handler,0,pack_size);
			}else{
				post_recv(*handler,cur_recv*pack_size,pack_size);
			}
			cur_recv+=1;
		}
		while(cur_complete<ops){
			ne = poll_recv_cq(*handler,wc);
			if(start_timer_flag==1 && ne!=0){
				clock_gettime(CLOCK_MONOTONIC, &start_timer);
				start_timer_flag=0;
			}
			for(int i=0;i<ne;i++){
				if(cur_recv<ops){
					if(is_same_addr){
						post_recv(*handler,0,pack_size);
					}else{
						post_recv(*handler,cur_recv*pack_size,pack_size);
					}
					cur_recv+=1;
				}
				assert(wc[i].status == IBV_WC_SUCCESS);
			}
			cur_complete+=ne;
		}
		clock_gettime(CLOCK_MONOTONIC, &end_timer);

		int verify_data_size = is_same_addr ? pack_size : ops*pack_size;
		for(int i=0;i<verify_data_size/sizeof(int);i++){
			if(((int*)buf)[i] != i){
				LOG_D("Data verification failed, index:%d data:%d expected_data:%d",i,((int*)buf)[i],i);
				break;
			}
		}
		LOG_I("Data verification success!");
	}
	double duration = end_timer.tv_sec-start_timer.tv_sec+1.0*(end_timer.tv_nsec-start_timer.tv_nsec)/1e9;
	LOG_I("Time : %.3f s",duration);
	LOG_I("Speed : %.2f Gb/s",8.0*ops*pack_size/1024/1024/1024/duration);
}
/*
@rc_send_recv_test
node 0 send 4 times 64 bytes data to node 1's four different places
*/
void rc_send_recv_test(UserParam &user_param, int num_qps){ 
	PingPongInfo *info = new PingPongInfo[user_param.numNodes * num_qps]();
	PingPongInfo **infos = new PingPongInfo*[user_param.numNodes];
	for(int i=0;i<user_param.numNodes;i++){
		infos[i] = &info[i*num_qps];
	}

	size_t buf_size = 1*1024*1024;
	void* buf = memalign(user_param.page_size,buf_size);

	QpHandler** handler = new QpHandler*[num_qps]();
	for(int i=0;i<num_qps;i++){
		handler[i] = create_qp_rc(user_param,buf,buf_size,infos[0]+i);
	}
    exchange_data(user_param, (char*)info, sizeof(PingPongInfo)*num_qps);
	for(int i=0;i<user_param.numNodes*num_qps;i++){
		print_pingpong_info(info+i);
	}

	if(user_param.numNodes == 2){//send/recv test
		for(int i=0;i<buf_size/sizeof(int);i++){
			if(user_param.nodeId == 0){
				((int*)buf)[i] = i;
			}else{
				((int*)buf)[i] = 0;
			}
		}

		int my_id = user_param.nodeId;
		int dest_id = (user_param.nodeId+1)%user_param.numNodes;
		for(int i=0;i<num_qps;i++){
			connect_qp_rc(user_param,*handler[i],infos[dest_id]+i,infos[my_id]+i);
		}
		
		
		int ne;
		struct ibv_wc *wc = NULL;
		ALLOCATE(wc ,struct ibv_wc ,CTX_POLL_BATCH);
		int ops = 4;
		int count;
		assert(ops*num_qps*64 <= buf_size);
		if(user_param.nodeId==0){
			for(int i=0;i<num_qps;i++){
				for(int j=0;j<ops;j++){
					post_send(*handler[i],(i*ops+j)*64,64);
				}
			}
			for(int i=0;i<num_qps;i++){
				count = ops;
				do{
					ne = poll_send_cq(*handler[i],wc);
					for(int i=0;i<ne;i++){
						LOG_D("WC_Status:%d",(int)wc[i].status);
						assert(wc[i].status == IBV_WC_SUCCESS);
						LOG_D("WC_ID:%d",(int)wc[i].wr_id);
					}
					count-=ne;
				}while(count>0);
			}
		}else{
			for(int i=0;i<num_qps;i++){
				for(int j=0;j<ops;j++){
					post_recv(*handler[i],(i*ops+j)*64,64);
				}
			}
			for(int i=0;i<num_qps;i++){
				count = ops;
				do{
					ne = poll_recv_cq(*handler[i],wc);
					for(int i=0;i<ne;i++){
						assert(wc[i].status == IBV_WC_SUCCESS);
						LOG_D("WC_ID:%d",(int)wc[i].wr_id);
					}
					count-=ne;
				}while(count>0);
			}
		}
		for(int i=0;i<(ops*num_qps*2*64)/sizeof(int);i++){
			cout<<((int*)buf)[i]<<" ";
			if((i+1)%16==0){
				cout<<endl;
			}
		}
	}
}

int main(int argc, char *argv[]){
	UserParam user_param;
	get_opt(user_param, argc, argv);
    socket_init(user_param);
    roce_init(user_param);
    // rc_send_recv_test(user_param,4);
	rc_send_recv_benchmark_single_qp(user_param,0,2048);
	return 0;
}