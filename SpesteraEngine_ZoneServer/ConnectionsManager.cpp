#include "ConnectionsManager.h"

void ConnectionsManager::add_new_connection(short id, boost::asio::ip::udp::endpoint endpoint)
{
	ZoneConnection zone_connection(endpoint);
}

void ConnectionsManager::delete_connection(short id)
{

}
