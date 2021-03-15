#include "mabx_data_udp.h"
#include "ttm_data_udp.h"
#include "logging/log.h"

MabxData::MabxData() : ttm_(std::make_unique<TtmData>()) {

}

bool MabxData::init(int rx_port, const std::string tx_address, int tx_port) {

    BaseSocket::init(rx_port, tx_address, tx_port);

    rx_thread_ = std::thread(&MabxData::receiveMabxData, this);

    rx_thread_native_handle_ = rx_thread_.native_handle();
    rx_thread_.detach();

    tx_thread_ = std::thread(&MabxData::transmitTtmDataToMabx, this);

    tx_thread_native_handle_ = tx_thread_.native_handle();
    tx_thread_.detach();

    return true;
}

void MabxData::receiveMabxData() {
    
    while(1)
    {
        UDPRecordBuffer_t data;

        // receive from mabx
        int msg_size = recvfrom(socket_fd_, &data, MAXLINE, MSG_WAITALL, (struct sockaddr *) &rx_address_, &ip_address_length_);

        if (msg_size > 0)
        {
            // add to TTM Tx queue
            if (ttm_) { 
                ttm_->pushTxBuffer(data); 
            }
            else { 
                LOG(WARNING) << "ttm object is null, dropping packet from MUDP\n";
            }
        }
    }

}

void MabxData::transmitTtmDataToMabx() {

    UDPRecordBuffer_t data;
    static int32_t update_index = 0;

    while (1)
    {
        std::cout.flush();

        // get from mabx (this) Tx queue (populated by TTM)
        if (takeFirstTxBuffer(data))
        {
            // transmit to mabx
            data.header.streamRefIndex = update_index;
            data.header.sourceTxCnt = update_index;
            data.header.sourceTxTime = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>
                                                    (std::chrono::system_clock::now().time_since_epoch()).count();

            int error_number = sendto(socket_fd_, (char*)&data.header, sizeof(data.header) + data.header.streamDataLen,
                                    MSG_CONFIRM, (const struct sockaddr *) &tx_address_, sizeof(tx_address_));
            if (error_number < 0)
            {
                LOG(ERROR) << "MUDP sendto: " << gai_strerror(error_number);
            }

            ++update_index;

        }
        
    }
    

}

bool MabxData::takeFirstTxBuffer(UDPRecordBuffer_t& udp_record) {

    if (tx_buffer_.empty()){
        return false;
    }

    std::lock_guard<std::mutex> lk(tx_buffer_mutex_);
    udp_record = tx_buffer_.front();
    tx_buffer_.pop();

    return true;
}

void MabxData::pushTxBuffer(const UDPRecordBuffer_t& udp_record) {
    std::lock_guard<std::mutex> lk(tx_buffer_mutex_);
    tx_buffer_.push(udp_record);

    if (tx_buffer_.size() > 1024){
        tx_buffer_.pop();
    }
}



bool MabxData::shutdown() {

    BaseSocket::shutdown();
    pthread_cancel(rx_thread_native_handle_);
    pthread_cancel(tx_thread_native_handle_);

    return true;
}

MabxData::~MabxData() {

}
