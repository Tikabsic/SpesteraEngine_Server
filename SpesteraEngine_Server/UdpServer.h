#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <boost/asio.hpp>
#include <unordered_map>
#include <mutex>

#include "NetworkProtocol.pb.h"

using boost::asio::ip::udp;

class UdpServer {
public:
    UdpServer(boost::asio::io_context& io_context, const std::string& address, int port);
    ~UdpServer();

    //Base
    std::unordered_map<u_short, udp::endpoint> get_endpoint_map();
    char* get_buffer();
    void receive_data();
    void response_to_login_request(udp::endpoint endpoint);

    //Deliver Type
    void send_data_to_all_players(const std::string& message);
    void send_data_to_other_players(const std::string& message, u_short playerId);

private:
    udp::socket socket_;
    udp::endpoint sender_endpoint_;
    enum { max_length = 512 * 1024 };
    char receive_data_[max_length];
    std::string send_buffer_;

    std::unordered_map<u_short, udp::endpoint> endpoint_map_;
    std::mutex endpoint_map_mutex_;
};

#endif // UDPSERVER_H