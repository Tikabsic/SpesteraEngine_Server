#include "ExternalConnection.h"
#include <iostream>

ExternalConnection::ExternalConnection(boost::asio::ip::tcp::socket socket, DbConnection* db_connection)
    : socket_(std::move(socket)), db_connection_(db_connection) {}

boost::asio::ip::tcp::socket& ExternalConnection::socket() {
    return socket_;
}

void ExternalConnection::start() {
    read_data();
}

void ExternalConnection::read_data() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::cout << "Message recived" << std::endl;
            }
            else {
                std::cerr << "Error on read: " << error.message() << std::endl;
            }
        });
}