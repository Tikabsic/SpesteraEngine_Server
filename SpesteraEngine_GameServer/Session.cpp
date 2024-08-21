#include "Session.h"
#include "TcpServer.h"
#include "DbServicesProvider.h"

#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver)
    : socket_(std::move(socket)), id_(id), tcp_server_(tcpserver), character_(nullptr) {
}

Session::~Session() {
    std::cout << "session closed" << std::endl;
}

void Session::start() {
    AssignId assign_id;
    assign_id.set_string(id_);

    GSWrapper wrapper;
    wrapper.set_type(GSWrapper::ASSIGNID);
    wrapper.set_payload(assign_id.SerializeAsString());

    direct_push_to_buffer(wrapper.SerializeAsString());
    do_read();
}

void Session::disconnect()
{

}

void Session::compress_to_write(const GSWrapper& msg) {
    bool write_in_progress = !write_msgs_.empty();
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(msg.SerializeAsString(), compressed_msg);
    write_msgs_.push_back(compressed_msg);
    if (!write_in_progress) {
        do_write();
    }
}

void Session::direct_push_to_buffer(const std::string& msg) {
    write_msgs_.push_back(msg);
    do_write();
}

void Session::set_player_id(u_short pid)
{
    playerId_ = pid;
}



void Session::handle_message(const GSWrapper& wrapper) {
    switch (wrapper.type()) {
    case GSWrapper::CLIENTLOGOUT: {
        std::cout << " Processing logout" << std::endl;
        tcp_server_->remove_session(shared_from_this());
        socket_.close();
        break;
    }
    case GSWrapper::ASSIGNID: {

        break;
    }
    default:
        std::cout << "Unknown message type" << std::endl;
        break;
    }
}

u_short Session::get_player_id()
{
    return playerId_;
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [ this, self ] (boost::system::error_code ec, std::size_t length) {
            if ( !ec ) {
                try {
                    GSWrapper wrapper;
                    if ( wrapper.ParseFromArray(data_, length) ) {
                        handle_message(wrapper);
                    }
                    do_read();
                }
                catch ( const std::runtime_error& e ) {
                    std::cerr << "Runtime error in do_read: " << e.what() << std::endl;
                    socket_.close();
                    tcp_server_->remove_session(shared_from_this());
                }
            }
            else {
                std::cout << "Connection break, closing socket..." << std::endl;
                tcp_server_->remove_session(shared_from_this());
                socket_.close();
            }
        });
}

void Session::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [ this, self ] (boost::system::error_code ec, std::size_t length) {
            if ( !ec ) {
                write_msgs_.pop_front();
                if ( !write_msgs_.empty() ) {
                    do_write();
                }
            }
        });
}