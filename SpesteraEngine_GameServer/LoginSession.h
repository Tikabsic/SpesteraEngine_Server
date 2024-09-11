#pragma once

#include "Session.h"
class LoginSession : public Session
{
public:
	LoginSession(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver);
	~LoginSession();

	void process_database_data(DatabaseResponseWrapper& wrapper) override;
private:
	//Account properties
	bool is_authenticated = false;

	//Message handlers
	void handle_message(const GSWrapperRequest& wrapper) override;
	void handle_connection_message(const ConnectionRequestWrapper& wrapper);

	//Database services handlers
	void handle_account_service_response(const AccountServiceResponseWrapper& wrapper) override;
	void handle_gamecharacter_service_response(const GameCharacterServiceResponseWrapper& wrapper) override;

	//Authentication
	void process_login_request(const RequestLogin& request);
	bool verify_login_request(const ResponseAccountData& data);

};

