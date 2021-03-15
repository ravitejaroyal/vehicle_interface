#pragma once

#include "ttm_data_udp.h"
#include "mabx_data_udp.h"
#include "logging/log.h"

TtmData::TtmData() : udp_(std::make_unique<MabxData>()) {

}

bool TtmData::init(int rx_port, const std::string tx_address, int tx_port) {

    BaseSocket::init(rx_port, tx_address, tx_port);

    rx_thread_ = std::thread(&TtmData::receiveTtmData, this);

    rx_thread_native_handle_ = rx_thread_.native_handle();
    rx_thread_.detach();

    tx_thread_ = std::thread(&TtmData::transmitMabxDataToTtm, this);

    tx_thread_native_handle_ = tx_thread_.native_handle();
    tx_thread_.detach();

    return true;
}

void TtmData::receiveTtmData() {

    char data[MAXLINE];
    
    while (1)
    {
        // receive from TTM backend
        int msg_size = recvfrom(socket_fd_, &data, MAXLINE, MSG_WAITALL, (struct sockaddr *) &rx_address_, &ip_address_length_);
        if (msg_size > 0)
        {
            // convert to UDP record and add to MUDP Tx queue
            json json_msg = json::parse(std::string(data, msg_size));
            UDPRecordBuffer_t parsed_data;
            if (jsonToUdpRecord(json_msg, parsed_data)) // be sure to populate sourceTxTime, streamRefIndex, sourceTxCnt later
            {
                if (udp_) {
                     udp_->pushTxBuffer(parsed_data);
                }
                else { 
                    LOG(WARNING) << "mudp object is null, dropping packet from MUDP\n";
                }
            }
        }
    }

}

void TtmData::transmitMabxDataToTtm() {

    UDPRecordBuffer_t parsed_data;

    json json_data;
    
    while (1)
    {
        std::cout.flush();

        // get from TTM (this) Tx queue (populated by mabx)
        if (takeFirstTxBuffer(parsed_data))
        {
            if (parsed_data.header.sourceInfo == ParkingInfrastructure::StreamSource_e::FUSION_PC)
            {
                // convert to json and transmit to TTM backend
                json_data = udpRecordToJSON(parsed_data);
                if (!json_data.is_null())
                {
                    int error_number = sendto(socket_fd_, json_data.dump().c_str(), json_data.dump().length(),
                                        MSG_CONFIRM, (const struct sockaddr *) &tx_address_, sizeof(tx_address_));
                    if (error_number < 0) { 
                        LOG(ERROR) << "TTM sendto: " << gai_strerror(error_number); 
                    }
                }
            }

        }

    }
}

bool TtmData::jsonToUdpRecord(const json& json_msg, UDPRecordBuffer_t& parsed_data) {

    parsed_data.header.versionInfo = UDP_RECORD_VERSIONINFO;
    parsed_data.header.streamChunks = 0;
    parsed_data.header.streamChunkIdx = 0;

    std::cout.flush();
    int msg_type = std::stoi((std::string)json_msg["msg_type"]);
    switch (msg_type)
    {
    case message_type::ttm_heartbeat:
        LOG(INFO) << "TTM - Received Heartbeat\n";
        namespace Heartbeat = ParkingInfrastructure::Enablement::Streams::Infrastructure::Heartbeat;

        parsed_data.header.sourceInfo = Heartbeat::STREAM_SOURCE;
        parsed_data.header.streamDataLen = sizeof(Heartbeat::Payload);
        parsed_data.header.streamNumber = Heartbeat::STREAM_NUMBER;
        parsed_data.header.streamVersion = Heartbeat::STREAM_VERSION;

        Heartbeat::Payload heartbeat_pay_load;
        heartbeat_pay_load.timestamp_ms = (uint64_t)std::stoull(std::string(json_msg["timestamp"]));
        heartbeat_pay_load.vehicleId = (uint64_t)199; // (uint64_t)std::stoull(std::string(json_msg["veh_id"]));

        memcpy(&parsed_data.payload, &heartbeat_pay_load, sizeof(heartbeat_pay_load));
        break;

    case message_type::ttm_localization:
        LOG(INFO) << "TTM - Received Localization\n";
        namespace Localization = ParkingInfrastructure::Localization::Streams::Infrastructure::Localization;

        parsed_data.header.sourceInfo = Localization::STREAM_SOURCE;
        parsed_data.header.streamDataLen = sizeof(Localization::Payload);
        parsed_data.header.streamNumber = Localization::STREAM_NUMBER;
        parsed_data.header.streamVersion = Localization::STREAM_VERSION;

        Localization::Payload loc_pay_load;
        loc_pay_load.timestamp_ms = (uint64_t)std::stoull(std::string(json_msg["meas_time"]));
        loc_pay_load.coordinateFrame.coordinateSystem = (ParkingInfrastructure::Localization::Types::CoordinateSystem_e)std::stoi(std::string(json_msg["frame"]));
        loc_pay_load.coordinateFrame.originZone = (uint32_t)std::stoul(std::string(json_msg["zone"]));

        loc_pay_load.state.x = std::stod(std::string(json_msg["X"]));
        loc_pay_load.state.y = std::stod(std::string(json_msg["Y"]));
        loc_pay_load.state.z_m = std::stod(std::string(json_msg["Z"]));
        loc_pay_load.state.roll_rad = std::stod(std::string(json_msg["roll"]));
        loc_pay_load.state.pitch_rad = std::stod(std::string(json_msg["pitch"]));
        loc_pay_load.state.yaw_rad = std::stod(std::string(json_msg["yaw"]));

        loc_pay_load.uncertainty.x = std::stod(std::string(json_msg["var_X"]));
        loc_pay_load.uncertainty.y = std::stod(std::string(json_msg["var_Y"]));
        loc_pay_load.uncertainty.z_m2 = std::stod(std::string(json_msg["var_Z"]));
        loc_pay_load.uncertainty.roll_rad2 = std::stod(std::string(json_msg["var_roll"]));
        loc_pay_load.uncertainty.pitch_rad2 = std::stod(std::string(json_msg["var_pitch"]));
        loc_pay_load.uncertainty.yaw_rad2 = std::stod(std::string(json_msg["var_yaw"]));

        memcpy(&parsed_data.payload, &loc_pay_load, sizeof(loc_pay_load));
        break;

    case message_type::ttm_routing:
        LOG(INFO) << "TTM - Received Routing\n";
        namespace Routing = ParkingInfrastructure::Routing::Streams::Infrastructure::Routing;

        parsed_data.header.sourceInfo = Routing::STREAM_SOURCE;
        parsed_data.header.streamDataLen = sizeof(Routing::Payload);
        parsed_data.header.streamNumber = Routing::STREAM_NUMBER;
        parsed_data.header.streamVersion = Routing::STREAM_VERSION;

        Routing::Payload routing_pay_load;
        routing_pay_load.timestamp_ms = (uint64_t)std::stoull(std::string(json_msg["timestamp"]));
        routing_pay_load.mode = (ParkingInfrastructure::Routing::Types::Mode_e)std::stoi(std::string(json_msg["mode"]));
        routing_pay_load.numberOfWaypoints = (uint16_t)std::stoul(std::string(json_msg["N"]));
        for (uint16_t i = 0; i < routing_pay_load.numberOfWaypoints; i++)
        {
            json json_msgChild = json_msg[std::to_string(i)];
            routing_pay_load.waypoints[i].index = (int32_t)std::stoull(std::string(json_msgChild["index"]));
            routing_pay_load.waypoints[i].x = std::stod(std::string(json_msgChild["X"]));
            routing_pay_load.waypoints[i].y = std::stod(std::string(json_msgChild["Y"]));
            routing_pay_load.waypoints[i].z_m = std::stod(std::string(json_msgChild["Z"]));
            routing_pay_load.waypoints[i].k = std::stod(std::string(json_msgChild["K"]));;
            routing_pay_load.waypoints[i].maxSpeed_mps = std::stod(std::string(json_msgChild["speed"]));
            routing_pay_load.waypoints[i].laneWidthRight_m = std::stod(std::string(json_msgChild["lanewidth_right"]));;
            routing_pay_load.waypoints[i].laneWidthLeft_m = std::stod(std::string(json_msgChild["lanewidth_left"]));;;
        }
        routing_pay_load.destinationWaypointIndex = (int32_t)std::stoul(std::string(json_msg["dest"]));

        memcpy(&parsed_data.payload, &routing_pay_load, sizeof(routing_pay_load));
        break;

    default:
        LOG(ERROR) << "Unrecognized message_type - Failed to parse JSON msg\n";
        return false;
    }

    return true;
}

json TtmData::udpRecordToJSON(const UDPRecordBuffer_t& udp_record) {

    json json_data = {};

    switch (udp_record.header.streamNumber)
    {
    case ParkingInfrastructure::Enablement::Streams::Vehicle::Heartbeat::STREAM_NUMBER:
        LOG(INFO) << "MUDP - Received Heartbeat\n";
        namespace Heartbeat = ParkingInfrastructure::Enablement::Streams::Vehicle::Heartbeat;
        Heartbeat::Payload heartbeat_pay_load;

        memcpy(&heartbeat_pay_load, &udp_record.payload, sizeof(Heartbeat::Payload));
        json_data["msg_type"] = std::to_string(message_type::vehicle_heartbeat);
        json_data["timestamp"] = std::to_string(heartbeat_pay_load.timestamp_ms);
        json_data["veh_id"] = std::to_string(heartbeat_pay_load.vehicleId);
        json_data["status"] = std::to_string(static_cast<uint8_t>(heartbeat_pay_load.vehicleStatus));
        break;
    case ParkingInfrastructure::Routing::Streams::Vehicle::Request::STREAM_NUMBER:
        LOG(INFO) << "MUDP - Received Reqest\n";
        namespace Request = ParkingInfrastructure::Routing::Streams::Vehicle::Request;
        Request::Payload reqPayload;

        memcpy(&reqPayload, &udp_record.payload, sizeof(Request::Payload));

        json_data["veh_id"] = std::to_string(199);
        json_data["type"] = std::to_string(static_cast<uint8_t>(reqPayload.requestType));
        break;

    default:
        LOG(ERROR) << "Unrecognized message_type - Failed to parse MUDP record\n";
    }

    return json_data;
}

bool TtmData::takeFirstTxBuffer(UDPRecordBuffer_t& udp_record) {

    if (tx_buffer_.empty()){
        return false;
    }

    std::lock_guard<std::mutex> lk(tx_buffer_mutex_);
    udp_record = tx_buffer_.front();
    tx_buffer_.pop();

    return true;
}

void TtmData::pushTxBuffer(const UDPRecordBuffer_t& udp_record) {
    std::lock_guard<std::mutex> lk(tx_buffer_mutex_);
    tx_buffer_.push(udp_record);

    /// only keep 1024 buffer in queue
    if (tx_buffer_.size() > 1024){
        tx_buffer_.pop();
    }
}

bool TtmData::shutdown() {

    BaseSocket::shutdown();
    pthread_cancel(rx_thread_native_handle_);
    pthread_cancel(tx_thread_native_handle_);

    return true;
}

TtmData::~TtmData() {

}
