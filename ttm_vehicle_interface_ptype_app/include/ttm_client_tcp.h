#pragma once

#include <stdio.h>      //for printf() and fprintf() 
#include <sys/socket.h> //for socket(), bind(), connect(), recv() and send()
#include <arpa/inet.h>  //for sockaddr_in and inet_ntoa()
#include <stdlib.h>     //for atoi() and exit() 
#include <string.h>     //for memeset()
#include <unistd.h>     //for close()
#include <iostream>
#include <string>

namespace ttmclient {

class TTMclientTCP {
public:

    TTMclientTCP(int32_t port, const std::string server_ip_address);
    bool connectRequest();
    bool sendData(const char* data, size_t data_length);
    int recvData(char* data);
    void shutdownSocket();
    ~TTMclientTCP() = default;

private:

    int32_t socket_fd_;
    struct sockaddr_in server_addr_port_;
    int port_;

};

}//namespace
