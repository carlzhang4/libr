
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include "src/core.hpp"
#include "src/util.hpp"
#include "src/connection_manager.hpp"

using namespace std;

/*
@rc_send_recv_test
node 0 send 4 times 64 bytes data to node 1's four different places
*/
void rc_send_recv_test(UserParam &user_param){ 
	PingPongInfo *info = new PingPongInfo[user_param.numNodes]();

	size_t buf_size = 1*1024*1024;
	void* buf = memalign(user_param.page_size,buf_size);

	QpHandler* handler = create_qp_rc(user_param,buf,buf_size,info);
    exchange_data(user_param, (char*)info, sizeof(PingPongInfo));
	for(int i=0;i<user_param.numNodes;i++){
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

		int my_index = user_param.nodeId;
		int dest_index = (user_param.nodeId+1)%user_param.numNodes;
		connect_qp_rc(user_param,*handler,info+dest_index,info+my_index);
		
		int ne;
		struct ibv_wc *wc = NULL;
		ALLOCATE(wc ,struct ibv_wc ,CTX_POLL_BATCH);

		if(user_param.nodeId==0){
			post_send(*handler,0,64);
			post_send(*handler,0,64);
			post_send(*handler,0,64);
			post_send(*handler,0,64);
			int count = 4;
			do{
				ne = poll_send_cq(*handler,wc);
				for(int i=0;i<ne;i++){
					LOG_D("WC_Status:%d",(int)wc[i].status);
					assert(wc[i].status == IBV_WC_SUCCESS);
					LOG_D("WC_ID:%d",(int)wc[i].wr_id);
				}
				count-=ne;
			}while(count>0);
		}else{
			post_recv(*handler,0,64);
			post_recv(*handler,64,64);
			post_recv(*handler,128,64);
			post_recv(*handler,192,64);
			int count = 4;
			do{
				ne = poll_recv_cq(*handler,wc);
				for(int i=0;i<ne;i++){
					assert(wc[i].status == IBV_WC_SUCCESS);
					LOG_D("WC_ID:%d",(int)wc[i].wr_id);
				}
				count-=ne;
			}while(count>0);
		}
		for(int i=0;i<512/sizeof(int);i++){
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
    rc_send_recv_test(user_param);
	return 0;
}