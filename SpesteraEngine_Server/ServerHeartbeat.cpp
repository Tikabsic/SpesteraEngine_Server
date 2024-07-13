#include "ServerHeartbeat.h"
#include "NetworkProtocol.pb.h"
#include "BinaryCompressor.h"
#include <chrono>

ServerHeartbeat::ServerHeartbeat(boost::asio::io_context& io_context, UdpServer& udpserver, int tickrate)
    : server_(udpserver),
    timer_(io_context, boost::asio::chrono::milliseconds(tickrate)),
    tickrate_(tickrate)
{
    do_heartbeat();
}

void ServerHeartbeat::do_heartbeat() {
    if (server_.get_endpoint_map().empty()) {
        std::cout << "Empty server" << std::endl;
        timer_.expires_after(boost::asio::chrono::milliseconds(tickrate_));
        timer_.async_wait([this](boost::system::error_code ec) {
            if (!ec) {
                do_heartbeat();
            }
            });
        return;
    }

    Heartbeat heartbeat;
    heartbeat.set_message("Heartbeat message");
    heartbeat.set_timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

    Wrapper wrapper;
    wrapper.set_channel(Wrapper_Channel_UDP);
    wrapper.set_type(Wrapper_MessageType_HEARTBEAT);

    std::string heartbeat_data;
    if (!heartbeat.SerializeToString(&heartbeat_data)) {
        std::cerr << "Failed to serialize heartbeat message" << std::endl;
        return;
    }
    wrapper.set_payload(heartbeat_data);

    std::string wrapper_data;
    if (!wrapper.SerializeToString(&wrapper_data)) {
        std::cerr << "Failed to serialize wrapper message" << std::endl;
        return;
    }

    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(wrapper_data, compressed_msg);

    server_.send_data_to_all_players(compressed_msg);

    timer_.expires_after(boost::asio::chrono::milliseconds(tickrate_));
    timer_.async_wait([this](boost::system::error_code ec) {
        if (!ec) {
            do_heartbeat();
        }
        });
}