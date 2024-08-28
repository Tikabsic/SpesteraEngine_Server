#include "ExternalConnection.h"
#include <iostream>

ExternalConnection::ExternalConnection(boost::asio::ip::tcp::socket socket, std::string dburi)
    : socket_(std::move(socket)) {
    std::cout << "External Connection established..." << std::endl;
    
    account_service_ = new AccountService(dburi);
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
                    DatabaseRequestWrapper wrapper;
                    if (wrapper.ParseFromString(std::string(recive_data_, bytes_transferred))) {
                        handle_message(wrapper);
                        read_data();
                    }
                    else {
                        std::cerr << "Failed to parse RequestWrapper" << std::endl;
                        read_data();
                    }
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

void ExternalConnection::handle_message(const DatabaseRequestWrapper& wrapper) {
	switch (wrapper.service_request_case()) {
	    case DatabaseRequestWrapper::kAccountServiceRequest: {
		    DatabaseResponseWrapper response = account_service_->handle_message(wrapper.account_service_request());
            response.set_session_id(wrapper.session_id());
		    push_and_send(response.SerializeAsString());
		    break;
	    }
	    case DatabaseRequestWrapper::kGamecharacterServiceRequest: {

		    break;
	    }
	}
}

void ExternalConnection::push_and_send(std::string message)
{
    bool is_writing = !send_data_.empty();
    
    send_data_.push_back(message);
    if (!is_writing) {
        send_data();
    }
}
