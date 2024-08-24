#include "Session.h"
#include "TcpServer.h"
#include "DbServicesProvider.h"
#include "DatabaseProtocol.pb.h"
#include "SSPHasher.h"

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
    std::cout << " Processing logout" << std::endl;
    tcp_server_->remove_session(shared_from_this());
    socket_.close();
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

    bool is_writing = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if ( !is_writing ) {
        do_write();
    }

}

void Session::set_player_id(u_short pid)
{
    playerId_ = pid;
}



void Session::handle_message(const GSWrapper& wrapper) {
    switch (wrapper.type()) {
    case GSWrapper::CLIENTLOGOUT: {
        disconnect();
        break;
    }
    case GSWrapper::REQUESTLOGIN: {
        RequestLogin request;
        if (request.ParseFromString(wrapper.payload()) ) {
            if ( request.account_name().empty() || request.password().empty() ) {
                LoginRequestResult result;
                result.set_validation_result(false);
                result.set_message("Username and password required!");

                GSWrapper wrapper;
                wrapper.set_type(GSWrapper::LOGINREQUESTRESULT);
                wrapper.set_payload(result.SerializeAsString());

                direct_push_to_buffer(wrapper.SerializeAsString());

                disconnect();

                break;
            }
            process_login_request(request);
        }
        break;
    }
    default:
        std::cout << "Unknown message type" << std::endl;
        break;
    }
}

void Session::process_database_data(ResponseWrapper& wrapper)
{
    switch ( wrapper.response_type() ) {
    case ResponseWrapper::RESPONSEACCOUNTDATA: {
        ResponseAccountData data;
        if ( data.ParseFromString(wrapper.response_payload()) ) {
            bool verify_result = verify_login_request(data);
            if ( verify_result ) {
                LoginRequestResult result;
                result.set_validation_result(verify_result);
                result.set_message("Succefully logged in...");

                GSWrapper wrapper;
                wrapper.set_type(GSWrapper::LOGINREQUESTRESULT);
                wrapper.set_payload(result.SerializeAsString());

                direct_push_to_buffer(wrapper.SerializeAsString());
                break;
            }
            else {
                LoginRequestResult result;
                result.set_validation_result(verify_result);
                result.set_message("Wrong username or password...");

                GSWrapper wrapper;
                wrapper.set_type(GSWrapper::LOGINREQUESTRESULT);
                wrapper.set_payload(result.SerializeAsString());

                direct_push_to_buffer(wrapper.SerializeAsString());
                break;
            }
        }
    }
    default:
        std::cout << "unknown response type" << std::endl;
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
        [self] (boost::system::error_code ec, std::size_t length) {
            if ( !ec ) {
                try {
                    GSWrapper wrapper;
                    if ( wrapper.ParseFromArray(self->data_, length) ) {
                        self->handle_message(wrapper);
                    }
                    self->do_read();
                }
                catch ( const std::runtime_error& e ) {
                    std::cerr << "Runtime error in do_read: " << e.what() << std::endl;
                    self->socket_.close();
                    self->tcp_server_->remove_session(self);
                }
            }
            else {
                std::cout << "Connection break, closing socket..." << std::endl;
                self->tcp_server_->remove_session(self);
                self->socket_.close();
            }
        });
}

void Session::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [ self ] (boost::system::error_code ec, std::size_t length) {
            if ( !ec ) {
                self->write_msgs_.pop_front();
                if ( !self->write_msgs_.empty() ) {
                    self->do_write();
                }
            }
        });
}

void Session::process_login_request(RequestLogin& request)
{
    RequestAccountData data;
    data.set_account_name(request.account_name());
    data.set_account_password_to_verify(request.password());
    RequestWrapper wrapper;
    wrapper.set_request_type(RequestWrapper::REQUESTACCOUNTDATA);
    wrapper.set_service_type(RequestWrapper::ACCOUNTSERVICE);
    wrapper.set_request_id(playerId_);
    wrapper.set_request_payload(data.SerializeAsString());

    auto& db_provider = DbServicesProvider::get_instance();
    db_provider.send_query(wrapper.SerializeAsString());
}

bool Session::verify_login_request(ResponseAccountData& data)
{
    SSPHasher hasher;
    bool verification_result = hasher.VerifyPassword(data.account_password(), data.account_password_to_verify());
    return verification_result;
}