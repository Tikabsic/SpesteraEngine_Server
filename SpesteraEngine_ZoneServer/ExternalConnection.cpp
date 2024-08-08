#include "ExternalConnection.h"
#include <iostream>

ExternalConnection::ExternalConnection(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)) {
    std::cout << "External Connection established..." << std::endl;
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
                try {
                    //RequestWrapper wrapper;
                    //if (wrapper.ParseFromString(std::string(recive_data_, bytes_transferred))) {
                    //    auto response = db_connection_->get_data(wrapper);
                    //    std::string data = response.SerializeAsString();
                    //    send_data_.push_back(data);
                    //    send_data();
                    //    read_data();
                    //}
                    //else {
                    //    std::cerr << "Failed to parse RequestWrapper" << std::endl;
                    //    read_data();
                    //}
                }
                catch (const std::runtime_error& e) {
                    std::cerr << "Runtime error: " << e.what() << std::endl;
                    read_data();
                }
            }
            else {
                std::cerr << "Error on read: " << error.message() << std::endl;
                read_data();
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
