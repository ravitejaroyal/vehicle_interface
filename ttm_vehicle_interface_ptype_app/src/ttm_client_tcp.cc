#include "ttm_client_tcp.h"
#include <iostream>
#include "logging/log.h"

namespace ttmclient {

TTMclientTCP::TTMclientTCP(int32_t port, const std::string server_ip_address) {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ >= 0) {
        LOG(INFO) << "Created TCP socket, fd: " << socket_fd_;
    }
    else {
        LOG(ERROR) << "Failed to create TCP socket";
    }
    memset(&server_addr_port_, 0, sizeof(server_addr_port_));
    server_addr_port_.sin_family = AF_INET;
    server_addr_port_.sin_port = htons(port);
    //inet_pton(AF_INET, server_ip_address.c_str(), &server_addr_port_.sin_addr);
    inet_aton(server_ip_address.c_str(), &server_addr_port_.sin_addr);

}

bool TTMclientTCP::connectRequest() {
    int16_t connect_result = connect(socket_fd_, (struct sockaddr*)&server_addr_port_, sizeof(server_addr_port_));
    if (connect_result < 0) {
        LOG(ERROR) << "TCP connect request failed";
        return false;
    }
    return true;
}

bool TTMclientTCP::sendData(const char* data, size_t data_length) {
    int32_t send_result = send(socket_fd_, data, data_length, 0);
    if (send_result < 0){
        LOG(ERROR) << "TCP send failed, number sent: " << send_result;
        return false;
    }
    else {
        LOG(INFO) << "TCP send successfull";
        return true;
    }

}

int TTMclientTCP::recvData(char* data) {
    constexpr int16_t max_data_size {32};
    int32_t bytes_recieved = recv(socket_fd_, data, max_data_size, 0);

    if (bytes_recieved < 0) {
        LOG(ERROR) << "TCP recieve failed, number read: " << bytes_recieved;
    }
    return bytes_recieved;
}

void TTMclientTCP::shutdownSocket() {
    close(socket_fd_);
}

}//namespace


