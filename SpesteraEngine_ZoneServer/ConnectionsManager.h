#pragma once
#include <map>
#include <boost/asio.hpp>

#include "ZoneConnection.h"

class ConnectionsManager
{
public:
	std::map<short, ZoneConnection> zone_connections_;

	void add_new_connection(short id, boost::asio::ip::udp::endpoint endpoint);
	void delete_connection(short id);
};

