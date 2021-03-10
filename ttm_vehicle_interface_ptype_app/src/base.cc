#include "base.h"
#include "logging/log.h"

Base::Base() {

}

bool Base::init(int rx_port, const char* tx_address, int tx_port) {

    int f_broadcast = 1;
    int opt_val = 1;
    int error_number = 0;

    socket_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
    if (socket_fd_ >= 0) {
        LOG(INFO) << "Created UDP socket, fd: " << socket_fd_;
    }
    else {
        LOG(ERROR) << "Failed to create UDP socket";
    }

    if ((error_number = setsockopt(socket_fd_, SOL_SOCKET, SO_BROADCAST, &f_broadcast, sizeof f_broadcast)) == -1) {

            LOG(ERROR) << "ERROR : setsockopt (SO_BROADCAST)" << gai_strerror(error_number) ;
            return false;
    }
    setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

    memset(&rx_address_, 0, sizeof(rx_address_));

    rx_address_.sin_family = AF_INET;
    rx_address_.sin_addr.s_addr = htonl(INADDR_ANY);
    rx_address_.sin_port = htons(rx_port);

    tx_address_.sin_family = AF_INET;
    tx_address_.sin_port = htons(tx_port);
    inet_aton(tx_address, &tx_address_.sin_addr);

    if (bind(socket_fd_, (const struct sockaddr *)&rx_address_, sizeof(rx_address_)) < 0)
    {
        LOG(ERROR) << "Failed to bind";
        return false;
    }

    return true;
}

bool Base::shutdown() {

    close(socket_fd_);

    return true;
}

Base::~Base() {

}


