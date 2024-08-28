#include "Session.h"

#include "DatabaseProtocol.pb.h"
#include "GameCharacterService.pb.h"
#include "AccountService.pb.h"

#include "SSPHasher.h"
#include "TcpServer.h"
#include "DbServicesProvider.h"
#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver)
	: socket_(std::move(socket)), id_(id), tcp_server_(tcpserver), character_(nullptr) {
}

Session::~Session() {
	std::cout << "session closed" << std::endl;
}

//Helpers

u_short Session::get_player_id()
{
	return playerId_;
}

void Session::start() {

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
	std::cout << " Processing logout" << std::endl;
	if ( !account_name_.empty() ) {
		tcp_server_->disconnect_account(account_name_);
	}
	tcp_server_->remove_session(shared_from_this());
	socket_.close();
}

void Session::set_player_id(u_short pid)
{
	playerId_ = pid;
}

//Message handling

void Session::handle_message(const GSWrapperRequest& wrapper) {
	if ( !is_authenticated ) {
		if ( wrapper.has_connection_request() &&
			wrapper.connection_request().action_case() == ConnectionRequestWrapper::kRequestLogin ) {
			const RequestLogin& request = wrapper.connection_request().request_login();
			if ( request.account_name().empty() || request.password().empty() ) {
				GSWrapperResponse wrapper_response;
				auto* connection_response_wrapper = wrapper_response.mutable_connection_response();
				connection_response_wrapper->mutable_login_result()->set_message("Username and/or password required!");

				direct_push_to_buffer(wrapper_response.SerializeAsString());

				disconnect();
				return;
			}
			process_login_request(request);
		}
		else {
			GSWrapperResponse wrapper_response;
			auto* connection_response_wrapper = wrapper_response.mutable_connection_response();
			connection_response_wrapper->mutable_login_result()->set_message("Unauthenticated, disconnecting...");

			direct_push_to_buffer(wrapper_response.SerializeAsString());

			disconnect();
		}
		return;
	}

	switch ( wrapper.request_case() ) {
	case GSWrapperRequest::kConnectionRequest: {
		disconnect();
		break;
	}
	default:
		std::cout << "Unknown message type" << std::endl;
		break;
	}
}

void Session::process_database_data(DatabaseResponseWrapper& wrapper)
{
	switch ( wrapper.service_response_case() ) {
	case DatabaseResponseWrapper::kAccountServiceResponse: {
		const auto& account_service_response = wrapper.account_service_response();
		handle_account_service_response(account_service_response);
		break;
	}
	case DatabaseResponseWrapper::kGamecharacterServiceResponse: {
		const auto& gamecharacter_service_response = wrapper.gamecharacter_service_response();
		handle_gamecharacter_service_response(gamecharacter_service_response);
		break;
	}
	default:
		std::cout << "unknown response type" << std::endl;
		break;
	}
}

//Database response handling

void Session::handle_account_service_response(const AccountServiceResponseWrapper& wrapper)
{
	switch ( wrapper.request_type() ) {
	case AccountServiceResponseWrapper::RESPONSEACCOUNTDATA: {
		const auto& data = wrapper.response_account_data();
		if ( tcp_server_->is_account_online(data.account_name()) ) {

			GSWrapperResponse wrapper;
			auto* connection_wrapper = wrapper.mutable_connection_response();
			connection_wrapper->mutable_login_result()->set_message("Account already log in...");
			direct_push_to_buffer(wrapper.SerializeAsString());
			disconnect();

			break;
		}

		bool verify_result = verify_login_request(data);

		if ( verify_result ) {

			GSWrapperResponse wrapper;
			auto* connection_wrapper = wrapper.mutable_connection_response();
			connection_wrapper->mutable_login_result()->set_message("Succesfully logged in ...");

			direct_push_to_buffer(wrapper.SerializeAsString());
			account_name_ = data.account_name();
			tcp_server_->connect_account(account_name_);
			is_authenticated = true;
			break;
		}
		else {

			GSWrapperResponse wrapper;
			auto* connection_wrapper = wrapper.mutable_connection_response();
			connection_wrapper->mutable_login_result()->set_message("Wrong username or password");

			direct_push_to_buffer(wrapper.SerializeAsString());
			disconnect();
			break;
		}
	}
	default:
		std::cerr << "Unknown response type" << std::endl;
		break;
	}
}

void Session::handle_gamecharacter_service_response(const GameCharacterServiceResponseWrapper& wrapper)
{

}

//Login handling methods

void Session::process_login_request(const RequestLogin& request)
{
	DatabaseRequestWrapper wrapper;
	wrapper.set_session_id(id_);
	auto* service_type = wrapper.mutable_account_service_request();
	auto* db_request = service_type->mutable_request_account_data();
	db_request->set_account_name(request.account_name());
	db_request->set_account_password_to_verify(request.password());

	auto& db_provider = DbServicesProvider::get_instance();
	db_provider.send_query(wrapper.SerializeAsString());
}

bool Session::verify_login_request(const ResponseAccountData& data)
{
	SSPHasher hasher;
	bool verification_result = hasher.VerifyPassword(data.account_password(), data.account_password_to_verify());
	return verification_result;
}

