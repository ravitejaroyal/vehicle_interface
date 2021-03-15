#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <netdb.h>

#include <queue>
#include <iostream>
#include <memory>

#include "base.h"
#include "udp_record.h"
#include "message_type.h"
#include "parking_infrastructure_streams.h"
#include "ttm_data_udp.h"

class TtmData;

class MabxData : public BaseSocket {
 public:
    MabxData();

    bool init(int rx_port, const std::string tx_address, int tx_port);

    void receiveMabxData();
    void transmitTtmDataToMabx();

    bool takeFirstTxBuffer(UDPRecordBuffer_t& udp_record);
    void pushTxBuffer(const UDPRecordBuffer_t& udp_record);

    bool shutdown();

    ~MabxData();

 private:
    std::queue<UDPRecordBuffer_t> tx_buffer_;
    std::mutex tx_buffer_mutex_;

    std::thread rx_thread_;
    pthread_t rx_thread_native_handle_;

    std::thread tx_thread_;
    pthread_t tx_thread_native_handle_;

    std::unique_ptr<TtmData> ttm_;
};







