#include <csignal>
#include <thread>
#include "ttm_client_tcp.h"
#include <iostream>
#include "logging/log.h"

#include "mabx_data_udp.h"
#include "ttm_data_udp.h"

constexpr int32_t port_ttm_initial{54000};
constexpr int32_t port_dat_fw{5000};
constexpr char ip_ttm[] {"127.0.0.1"};
constexpr char ip_dat_fw[] {"10.0.0.193"};
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

    std::string ttm_connect_msg = ttm_vehicle_id;
    ttmStartupClient.sendData(ttm_connect_msg.c_str(), ttm_connect_msg.length());

    char port_msg[32];
    int port_msg_size = -1;
    while((port_msg_size = ttmStartupClient.recvData((char*)&port_msg)) <= 0) {
        LOG(INFO) << "Waiting to recieve port number for UDP connection";

    }
    
    std::string ttm_rx_port = port_msg;
    LOG(DEBUG) << "TTM listen port: " << ttm_rx_port;
    
    ttmStartupClient.shutdownSocket();
    LOG(INFO) << "Shutdown ttm tcp socket";

    MabxData udp;
    TtmData ttm;

    if (!udp.init(port_dat_fw, ip_dat_fw, port_dat_fw)) {
         LOG(DEBUG) << "MABX init fail: " << std::endl;
        return -1;
    }

    if (!ttm.init(std::stoi(ttm_rx_port), ip_ttm, std::stoi(ttm_rx_port)+1)) {
        LOG(DEBUG) << "TTM init fail: " << std::endl;
        return -1;
    }
    
           // important -- in lieu of joining other threads here, just keep main thread active indefinitely
    while(1)
    {
        if (exitFlag)
        {
            std::cout << "shutdown" << std::endl;
            ttm.shutdown();
            udp.shutdown();
            // set LED color back to red
            //led.setColor(0xff,0x00);
            //LED::msExecDelay(100);
            break;
        }

        // sleeping turns main into a waitable thread instead of runnable
        // (runnable consumes a CPU core, waitable does not)
        std::this_thread::sleep_for(std::chrono::milliseconds(default_main_sleep_ms));
    }
}
