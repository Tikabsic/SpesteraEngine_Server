#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <boost/asio.hpp>
#include <unordered_map>
#include <mutex>

using boost::asio::ip::udp;

class UdpServer {
public:
    UdpServer(boost::asio::io_context& io_context, const std::string& address, int port);
    ~UdpServer();

    std::unordered_map<std::string, udp::endpoint> get_endpoint_map();
    char* get_buffer();
    void receive_data();
    void response(udp::endpoint endpoint);
    void send_data_to_all_players(const std::string& message);

private:
    udp::socket socket_;
    udp::endpoint sender_endpoint_;
    enum { max_length = 512 * 1024 };
    char receive_data_[max_length];
    std::string send_buffer_;

    std::unordered_map<std::string, udp::endpoint> endpoint_map_;
    std::mutex endpoint_map_mutex_;
};

#endif // UDPSERVER_H