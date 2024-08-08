#include "ConnectionsManager.h"

void ConnectionsManager::add_new_connection(short id, boost::asio::ip::udp::endpoint endpoint)
{
	connections_.insert({id, endpoint});
}

void ConnectionsManager::delete_connection(short id)
{
	connections_.erase(id);
}

std::map<short, boost::asio::ip::udp::endpoint>& ConnectionsManager::get_connections()
{
	return connections_;
}

bool ConnectionsManager::is_anyone_online()
{
	return !connections_.empty();
}
