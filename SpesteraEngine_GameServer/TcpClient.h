#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <deque>
#include <mutex>

using boost::asio::ip::tcp;

class TcpClient {
public:
    TcpClient(boost::asio::io_context& io_context, const std::string& address, unsigned short port);

    void connect();
    void send(const std::string& message);

private:
    void do_connect(const tcp::resolver::results_type& endpoints);
    void do_write(const std::string& message);
    void do_read();
    void handle_read(const boost::system::error_code& ec, std::size_t length);
    void handle_write(const boost::system::error_code& ec, std::size_t length);

    boost::asio::io_context& io_context_;
    tcp::resolver resolver_;
    tcp::socket socket_;
    std::string address_;
    unsigned short port_;
    std::deque<std::string> write_msgs_;
    std::mutex write_msgs_mutex_;
    enum { max_length = 1024 };
    char read_msg_[max_length];
};

#endif // TCP_CLIENT_H