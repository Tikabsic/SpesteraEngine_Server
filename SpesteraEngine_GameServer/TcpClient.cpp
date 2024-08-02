#include "TcpClient.h"

TcpClient::TcpClient(boost::asio::io_context& io_context, const std::string& address, unsigned short port)
    : io_context_(io_context), resolver_(io_context), socket_(io_context), address_(address), port_(port) {
    std::cout << "New TcpClient instantiated" << std::endl;
    connect();
}

void TcpClient::connect() {
    tcp::resolver::query query(address_, std::to_string(port_));
    resolver_.async_resolve(query,
        [this] (const boost::system::error_code& ec, tcp::resolver::results_type endpoints) {
            if ( !ec ) {
                do_connect(endpoints);
            }
            else {
                std::cerr << "Resolve error: " << ec.message() << std::endl;
            }
        });
}

void TcpClient::do_connect(const tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(socket_, endpoints,
        [this] (const boost::system::error_code& ec, const tcp::endpoint& /*endpoint*/) {
            if ( !ec ) {
                std::cout << "TcpClient connection established!" << std::endl;
                do_read();
            }
            else {
                std::cerr << "Connect error: " << ec.message() << std::endl;
            }
        });
}

void TcpClient::send(const std::string& message) {
    std::lock_guard<std::mutex> lock(write_msgs_mutex_);
    write_msgs_.push_back(message);
    if ( write_msgs_.size() > 1 ) {
        return;
    }
    do_write(write_msgs_.front());
}

void TcpClient::do_write(const std::string& message) {
    boost::asio::async_write(socket_, boost::asio::buffer(message.data(), message.size()),
        [this] (const boost::system::error_code& ec, std::size_t length) {
            handle_write(ec, length);
        });
}

void TcpClient::do_read() {
    socket_.async_read_some(boost::asio::buffer(read_msg_, max_length),
        [ this ] (const boost::system::error_code& ec, std::size_t length) {
            handle_read(ec, length);
        });
}

void TcpClient::handle_read(const boost::system::error_code& ec, std::size_t length) {
    if ( !ec ) {
        std::cout.write(read_msg_, length);
        std::cout << "\n";
        do_read();
    }
    else {
        std::cerr << "Read error: " << ec.message() << std::endl;
    }
}

void TcpClient::handle_write(const boost::system::error_code& ec, std::size_t /*length*/) {
    if ( !ec ) {
        std::lock_guard<std::mutex> lock(write_msgs_mutex_);
        write_msgs_.pop_front();
        if ( !write_msgs_.empty() ) {
            do_write(write_msgs_.front());
        }
    }
    else {
        std::cerr << "Write error: " << ec.message() << std::endl;
    }
}