
#include <thread>
#include <cstdio>
#include "src/nano_coroutine.hpp"
#include "src/connection_manager.hpp"

// Pass Parameters Like Normal Function
auto asymmetricTransferCoroutine(int processId, char* bufferOutput) -> nano_coroutine::AsyncTask {
    for (int i = 0; i < 2; i++) {
        sprintf(bufferOutput, "[%d]: This is the message from asymmetric-transfered coroutine\n", i);
        ASYMMETRIC_TRANSFER();  // Asymmetric Transfer, Return To the Caller
        // Resume Here
        printf("ProcessId[%d]: Resumed!\n", processId);
    }
    co_return;
}

auto symmetricTransferCoroutine(int processId, int coroutineId) -> nano_coroutine::AsyncTask {
    for (int i = 0; i < 2; i++) {
        printf("ProcessId: %d, CoroutineId: %d, ExecutionCount: %d\n", processId, coroutineId, i);
        SYMMETRIC_TRANSFER();   // Symmetric Transfer, Continue To Next Coroutine In ReadyTaskPool
        // Resume Here
    }
    co_return;
}

int main(int argc, char *argv[]) {
    using namespace std;
    if (argc < 4) {
        printf("Too Few Arguments...\n");
        printf("Usage: %s [processId] [serverIp] [totalProcess]\n", argv[0]);
        printf("Example: %s 0 192.168.8.8 2\n", argv[0]);
        return 1;
    }
    // Deal With Command Line Parameters
    auto processId = stoi(string(argv[1]));
    auto serverIp = string(argv[2]);
    auto totalProcess = stoi(string(argv[3]));
    auto syncDataLength = sizeof(decltype(processId));
    printf("ProcessId: %d, ServerIp: %s, TotalProcess: %d\n", processId, serverIp.c_str(), totalProcess);
    // Sync Server Process
    // C++ thread (Since CXX11)
    thread serverThread{};
    if (processId == 0) {
        // Create Sync Server Thread To Avoid Blocking
        serverThread = thread(syncServer, syncDataLength, totalProcess);
        // ******* NOTICE *******
        // When Passing Refrence To Thread Function, Use std::ref()
        // For Example:
        // void Function(int a, int& b, int c);
        // thread(Function, a, std::ref(b), c) ........ Right ^_^
        // thread(Function, a, b, c) .................. Wrong @_@
    }
    sleep(1);   // Waiting For Server Start Up
    // Sync Client Start
    decltype(processId) *receivedProcessId{nullptr};
    sync(
        reinterpret_cast<void*>(&processId),
        reinterpret_cast<void**>(&receivedProcessId),
        syncDataLength,
        serverIp,
        totalProcess);
    // Finish Sync And Join Server Thread
    if (processId == 0) {
        serverThread.join();
    }
    // Validate Synced Data
    for (int i = 0; i < totalProcess - 1; i++) {
        cout << "Synced Data: " << receivedProcessId[i] << endl;
    }
    char bufferOutput[100]{};
    // Create Coroutine And Get Coroutine Handler
    // Asymmetric Transfer Coroutine
    auto coroutineHandler = asymmetricTransferCoroutine(processId, bufferOutput);       // Pass Parameters Like Normal Function
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
        nano_coroutine::taskPool[i] = symmetricTransferCoroutine(processId, i);
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
}