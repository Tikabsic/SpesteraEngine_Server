#include "LoginSession.h"

#include "DatabaseProtocol.pb.h"
#include "GameCharacterService.pb.h"
#include "AccountService.pb.h"

#include "SSPHasher.h"
#include "TcpServer.h"
#include "DbServicesProvider.h"
#include <iostream>

LoginSession::LoginSession(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver)
	: Session(std::move(socket), id, tcpserver)
{
	std::cout << "LoginSession created" << std::endl;
}

LoginSession::~LoginSession()
{
	std::cout << "LoginSession deleted" << std::endl;
}

//Message handling

void LoginSession::handle_message(const GSWrapperRequest& wrapper) {
	start_time = timer_.now();
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
		handle_connection_message(wrapper.connection_request());
		break;
	}
	default:
		std::cout << "Unknown message type" << std::endl;
		break;
	}
}

void LoginSession::handle_connection_message(const ConnectionRequestWrapper& wrapper)
{
	switch ( wrapper.action_case() ) {
		case ConnectionRequestWrapper::kClientLogout: {
			disconnect();
			break;
		}
	}
}

//Database handling

void LoginSession::process_database_data(DatabaseResponseWrapper& wrapper)
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

void LoginSession::handle_account_service_response(const AccountServiceResponseWrapper& wrapper)
{
	switch ( wrapper.response_case() ) {
	case AccountServiceResponseWrapper::kResponseAccountData: {
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

		std::cout << verify_result << std::endl;

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

void LoginSession::handle_gamecharacter_service_response(const GameCharacterServiceResponseWrapper& wrapper)
{

}

//Login handling methods

void LoginSession::process_login_request(const RequestLogin& request)
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

bool LoginSession::verify_login_request(const ResponseAccountData& data)
{
	if ( data.account_name().empty() ) {
		return false;
	}

	SSPHasher hasher;
	bool verification_result = hasher.VerifyPassword(data.account_password(), data.account_password_to_verify());
	return verification_result;
}