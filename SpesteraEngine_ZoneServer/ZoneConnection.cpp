#include "ZoneConnection.h"

ZoneConnection::ZoneConnection(boost::asio::ip::udp::endpoint endpoint) :
	endpoint_(endpoint)
{

}

const boost::asio::ip::udp::endpoint& ZoneConnection::get_endpoint() const
{
	return endpoint_;
}
