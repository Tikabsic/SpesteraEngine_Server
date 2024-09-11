#include "Session.h"

#include "TcpServer.h"
#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver)
	: socket_(std::move(socket)), id_(id), tcp_server_(tcpserver), character_(nullptr) {
}

//Helpers

u_short Session::get_player_id()
{
	return id_;
}

void Session::start() {
	start_time = timer_.now();
	GSWrapperResponse wrapper;
	auto* connection_response = wrapper.mutable_connection_response();
	auto* assignid = connection_response->mutable_assign_id();
	assignid->set_string(id_);

	direct_push_to_buffer(wrapper.SerializeAsString());
	do_read();
}

//Basic IO functions

void Session::do_write() {
	auto self(shared_from_this());
	end_time = timer_.now();
	auto duration = end_time - start_time;
	auto microseconds = std::chrono::duration_cast< std::chrono::microseconds >( duration ).count();
	std::cout << "Czas przetwarzania: " << microseconds << " mikrosekund" << std::endl;
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

void Session::do_read() {
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(data_, max_length),
		[ self ] (boost::system::error_code ec, std::size_t length) {
			if ( !ec ) {
				try {
					GSWrapperRequest wrapper;
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

void Session::compress_to_write(const GSWrapperResponse& msg) {
	bool write_in_progress = !write_msgs_.empty();
	std::string compressed_msg;
	BinaryCompressor compressor;
	compressor.compress_string(msg.SerializeAsString(), compressed_msg);
	write_msgs_.push_back(compressed_msg);
	if ( !write_in_progress ) {
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

void Session::disconnect()
{
	if ( !account_name_.empty() ) {
		tcp_server_->disconnect_account(account_name_);
	}
	tcp_server_->remove_session(shared_from_this());
	socket_.close();
}





