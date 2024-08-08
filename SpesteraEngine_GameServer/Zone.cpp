#include "Zone.h"

Zone::Zone(boost::asio::io_context& io_context, unsigned short port, std::string address) :
	io_context_(io_context), port_(port), zone_(get_zone_name_from_port(port))
{
	tcp_client_ = std::make_unique<TcpClient>(io_context_, address, port, Client_Type::ZONE_SERVER_CONNECTION);
}

Zone::~Zone()
{
}

Zone_Name Zone::get_zone_name_from_port(unsigned short port)
{
	switch ( port ) {
	case 7172: return MAIN;
	default: throw std::invalid_argument("Unknown port for zone");
	}
}
