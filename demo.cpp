
#include <thread>
#include <cstdio>
#include <stdio.h>
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

    LOG_I("nodeId:%d",user_param.nodeId);
}

struct Test{
	int					id;
    char                s;
};

int main(int argc, char *argv[]) {
   
	UserParam user_param;
	get_opt(user_param, argc, argv);
    socket_init(user_param);

    Test *a = new Test[user_param.numNodes]();
    a[0].id = user_param.nodeId;
    a[0].s = 'x';
    exchange_data(user_param, (char*)a, sizeof(Test));
    for(int i=0;i<user_param.numNodes;i++){
        cout<<a[i].id << " " <<a[i].s <<endl;
    }
    

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