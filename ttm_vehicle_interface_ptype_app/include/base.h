#pragma once

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "udp_record.h"
#include "message_type.h"
#include "parking_infrastructure_streams.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

#ifndef MAXLINE
#define MAXLINE 30000
#endif


class Base {
 public:
    Base();
    bool init(int rx_port, const char* tx_address, int tx_port);
    virtual void receiveThreadProcess()=0;
    virtual void transmitThreadProcess()=0;
    bool shutdown();
    virtual ~Base();

 protected:
    int socket_fd_;
    struct sockaddr_in rx_address_;
    socklen_t ip_address_length_;
    struct sockaddr_in tx_address_;
};

