#include <csignal>
#include <thread>
#include "ttm_client_tcp.h"
#include <iostream>
#include "logging/log.h"

constexpr int32_t port_ttm_initial{54000};
constexpr char ip_ttm[] {"127.0.0.1"};
constexpr int16_t default_main_sleep_ms{100};
constexpr char ttm_vehicle_id[] {"199"};
constexpr int16_t exit_signal{2};

volatile sig_atomic_t exitFlag = false;

void signalHandler(int signal) {
    LOG(INFO) << "Interrupt signal recieved.";
    if( signal == exit_signal ) {
        exitFlag = true;
    }
}

int main(int argc, char** argv) {
    ::signal(SIGINT, signalHandler);
    logging::Logger::initialize();
    ttmclient::TTMclientTCP ttmStartupClient(port_ttm_initial, ip_ttm);
    while( !ttmStartupClient.connectRequest() ) {
        if (exitFlag) {
            LOG(INFO) << "Shutdown ttm tcp socket";
            ttmStartupClient.shutdownSocket();
            return 0;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(default_main_sleep_ms));
    }

    //led.setColor(0x00,0xff); // set LED color to green

    //std::string ttm_connect_msg = json({{"veh_id", ttm_vehicle_id}}).dump();
    /*json j_veh_id = 
    { 
        {"veh_id", TTM_VEH_ID }
    };*/
    //std::string ttmConnectMsg = j_veh_id.value("veh_id", "none");
    std::string ttm_connect_msg = ttm_vehicle_id;
    ttmStartupClient.sendData(ttm_connect_msg.c_str(), ttm_connect_msg.length());

    char port_msg[32];
    int port_msg_size = -1;
    while((port_msg_size = ttmStartupClient.recvData((char*)&port_msg)) <= 0) {
        LOG(INFO) << "Waiting to recieve port number for UDP connection";

    }
    //std::string ttm_rx_port = json::parse(std::string(port_msg, port_msg_size))["port"];
    std::string ttm_rx_port = port_msg;
    LOG(DEBUG) << "TTM listen port: " << ttm_rx_port;
    
    ttmStartupClient.shutdownSocket();
    LOG(INFO) << "Shutdown ttm tcp socket";
    return 0;
}

