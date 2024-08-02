#include "ExternalConnection.h"
#include <iostream>

ExternalConnection::ExternalConnection(boost::asio::ip::tcp::socket socket, DbConnection* db_connection)
    : socket_(std::move(socket)), db_connection_(db_connection) {
    std::cout << "New ExternalConnection established" << std::endl;
}

boost::asio::ip::tcp::socket& ExternalConnection::socket() {
    return socket_;
}

void ExternalConnection::start() {
    read_data();
}

void ExternalConnection::read_data() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(recive_data_, max_length),
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::cout << "Message recived" << std::endl;
            }
            else {
                std::cerr << "Error on read: " << error.message() << std::endl;
            }
        });
}

void ExternalConnection::send_data()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(send_data_.front()),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                send_data_.pop_front();
                if (!send_data_.empty()) {
                    send_data();
                }
            }
        });
}
