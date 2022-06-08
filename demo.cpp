
#include <iostream>
#include <thread>
#include <cstdio>
#include <stdio.h>
#include "src/core.hpp"
#include "src/util.hpp"
#include "src/nano_coroutine.hpp"
#include "src/connection_manager.hpp"

// Pass Parameters Like Normal Function
auto asymmetricTransferCoroutine(int nodeId, char* bufferOutput) -> nano_coroutine::AsyncTask {
    for (int i = 0; i < 2; i++) {
        sprintf(bufferOutput, "[%d]: This is the message from asymmetric-transfered coroutine\n", i);
        ASYMMETRIC_TRANSFER();  // Asymmetric Transfer, Return To the Caller
        // Resume Here
        printf("nodeId[%d]: Resumed!\n", nodeId);
    }
    co_return;
}

auto symmetricTransferCoroutine(int nodeId, int coroutineId) -> nano_coroutine::AsyncTask {
    for (int i = 0; i < 2; i++) {
        printf("nodeId: %d, CoroutineId: %d, ExecutionCount: %d\n", nodeId, coroutineId, i);
        SYMMETRIC_TRANSFER();   // Symmetric Transfer, Continue To Next Coroutine In ReadyTaskPool
        // Resume Here
    }
    co_return;
}

using namespace std;

void get_opt(UserParam &user_param,int argc, char* argv[]){
	int opt;
	const char *optstring = "n:i:s:";
	while((opt = getopt(argc,argv,optstring)) != -1){
		switch (opt){
		case 'n':
			user_param.numNodes = stoi(optarg);
			break;
		case 'i':
			user_param.nodeId = stoi(optarg);
			break;
		case 's':
			user_param.serverIp = string(optarg);
			break;
		default:
			LOG_E("Unknow parameter");
		}
	}
    if(user_param.nodeId == 0){
        user_param.sockfd = new int[user_param.numNodes];//zero is left unused
    }else{
        user_param.sockfd = new int;
    }

    user_param.ib_port = 1;//minimum 1
    user_param.gid_index = 2;//minimum 1
    user_param.page_size = sysconf(_SC_PAGESIZE);
    user_param.cacheline_size = get_cache_line_size();

    LOG_I("nodeId:%d numNodes:%d",user_param.nodeId,user_param.numNodes);
}


int main(int argc, char *argv[]) {
	UserParam user_param;
	get_opt(user_param, argc, argv);
    socket_init(user_param);
    roce_init(user_param);
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
			post_send(*handler,0,256);
			do{
				ne = poll_send_cq(*handler,wc);
			}while(ne==0);
			for(int i=0;i<ne;i++){
				cout<<wc[i].status<<endl;
				assert(wc[i].status == IBV_WC_SUCCESS);
				cout<<"WC_ID:"<<(int)wc[i].wr_id<<endl;
			}
		}else{
			post_recv(*handler,0,256);
			do{
				ne = poll_recv_cq(*handler,wc);
			}while(ne==0);
			for(int i=0;i<ne;i++){
				assert(wc[i].status == IBV_WC_SUCCESS);
				cout<<"WC_ID:"<<(int)wc[i].wr_id<<endl;
			}
		}
	}
	return 0;

    char bufferOutput[100]{};
    // Create Coroutine And Get Coroutine Handler
    // Asymmetric Transfer Coroutine
    auto coroutineHandler = asymmetricTransferCoroutine(user_param.nodeId, bufferOutput);       // Pass Parameters Like Normal Function
    // Resume To Execute Coroutine
    coroutineHandler.resume();
    cout << bufferOutput;
    coroutineHandler.resume();
    cout << bufferOutput;
    coroutineHandler.resume();
    // Coroutine Finished
    // ******* NOTICE *******
    // When Coroutine Handler Object(Like `coroutineHandler` Above) Was Destroyed(Deconstructed),
    // The Coroutine Will Be Destroyed Accordingly, No Matter Whether It Has Finished.
    // Coroutine Handler Object Can Only Be Moved, All Copy Semantics Has Been Disabled.

    for (int i = 0; i < MAX_COROUTINE_NUMBER; i++) {
        // Every Thread Has Its Own TaskPool And ReadyTaskPool,
        // Therefore, It Is Thread-Safe
        nano_coroutine::taskPool[i] = symmetricTransferCoroutine(user_param.nodeId, i);
    }
    
    {
        using namespace nano_coroutine;
        // When Adding TaskId To ReadyTaskPool, ReadyIndex Begin With 0
        readyTaskPool[readyIndex++] = 2;    // The Third Coroutine
        readyTaskPool[readyIndex++] = 0;    // The First Coroutine
        readyTaskPool[readyIndex++] = 9;    // The Last Coroutine
        // Don't Forget To Decrease ReadyIndex By 1 
        // When Applying Symmetric Transfer, ReadyIndex Decrease And Finally Be 0 When Return To Caller
        taskPool[readyTaskPool[--readyIndex]].resume();

        readyTaskPool[readyIndex++] = 9;
        readyTaskPool[readyIndex++] = 1;
        readyTaskPool[readyIndex++] = 3;
        taskPool[readyTaskPool[--readyIndex]].resume();
    }
    
    cout << "Exited...." << endl;
	fflush(stdout);
	return 0;
}