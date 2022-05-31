#include "connection_manager.hpp"
#include "util.hpp"
void sync(void* sendData, void** receivedData, uint64_t sendDataLength) {
    addrinfo hints{};
    // Initialize Server Info
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* serverAddress{nullptr};
    getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &serverAddress);
    // Connect To Server
    auto socketToServer = socket(serverAddress->ai_family, serverAddress->ai_socktype, serverAddress->ai_protocol);
    auto isConnected = connect(socketToServer, serverAddress->ai_addr, serverAddress->ai_addrlen);
    freeaddrinfo(serverAddress);
    // Send Sync Data
    send(socketToServer, sendData, sendDataLength, 0);
    uint64_t readBytes = 0;
    uint64_t receivedDataLength = (CLIENT_NUMBER - 1) * sendDataLength;
    *receivedData = reinterpret_cast<void*>(new uint8_t[receivedDataLength]);
    // Receive Sync Data
    while (readBytes != receivedDataLength) {
        readBytes += recv(socketToServer, (*receivedData) + readBytes, receivedDataLength - readBytes, 0);
    }
    close(socketToServer);
}

void syncServer(uint64_t receivedDataLength) {
    addrinfo hints{};
    // Initialize Server Info
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    addrinfo* serverBindAddress{nullptr};
    getaddrinfo(NULL, SERVER_PORT, &hints, &serverBindAddress);
    auto socketListen = socket(serverBindAddress->ai_family, serverBindAddress->ai_socktype, serverBindAddress->ai_protocol);
    // Bind The Socket
    auto isBound = bind(socketListen, serverBindAddress->ai_addr, serverBindAddress->ai_addrlen);
    // Start Listening Port
    auto isListening = listen(socketListen, 1024);
    decltype(socketListen) socketClients[CLIENT_NUMBER]{};
    void* syncData[CLIENT_NUMBER]{};
    freeaddrinfo(serverBindAddress);
    // Receive Sync Data
    for (int i = 0; i < CLIENT_NUMBER; i++) {
        // Accept Connection
        sockaddr_storage connectionAddress;
        socklen_t connectionLength = sizeof(connectionAddress);
        auto socketConnection = accept(socketListen, (sockaddr*)&connectionAddress, &connectionLength);
        socketClients[i] = socketConnection;
        syncData[i] = reinterpret_cast<void*>(new uint8_t[receivedDataLength]);
        // Receive Sync Data
        uint64_t readBytes = 0;
        while (readBytes != receivedDataLength) {
            readBytes += recv(socketConnection, syncData[i]+readBytes, receivedDataLength-readBytes, 0);
        }
    }

    // Sent Sync Data
    for (int i = 0; i < CLIENT_NUMBER; i++) {
        auto socketConnection = socketClients[i];
        for (int j = 0; j < CLIENT_NUMBER; j++) {
            if (j != i) {
                send(socketConnection, syncData[j], receivedDataLength, 0);
            }
        }
        close(socketConnection);
    }

    close(socketListen);
    for (int i = 0; i < CLIENT_NUMBER; i++) {
        delete [] reinterpret_cast<uint8_t*>(syncData[i]);
    }
}