#pragma once
#include <map>
#include <boost/asio.hpp>

class ConnectionsManager : public std::enable_shared_from_this<ConnectionsManager>
{
public:
	void add_new_connection(short id, boost::asio::ip::udp::endpoint endpoint);
	void delete_connection(short id);

	std::map<short, boost::asio::ip::udp::endpoint>& get_connections();

	bool is_anyone_online();

private:
	std::map<short, boost::asio::ip::udp::endpoint> connections_;
};

