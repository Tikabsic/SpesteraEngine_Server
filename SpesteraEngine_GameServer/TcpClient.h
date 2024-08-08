#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <deque>
#include <mutex>

using boost::asio::ip::tcp;

enum Client_Type {
    DB_SERVICE_PROVIDER = 0,
    ZONE_SERVER_CONNECTION =1
};

class TcpClient {
public:
    TcpClient(boost::asio::io_context& io_context, const std::string& address, unsigned short port, Client_Type clienttype);

    void connect();
    void send(const std::string& message);

private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_write();
    void do_read();

    boost::asio::io_context& io_context_;
    tcp::resolver resolver_;
    tcp::socket socket_;
    std::string address_;
    unsigned short port_;
    boost::asio::steady_timer reconnect_timer_;
    std::deque<std::string> write_msgs_;
    std::mutex write_msgs_mutex_;
    enum { max_length = 1024 };
    char read_msg_[max_length];

    Client_Type type_;
};
