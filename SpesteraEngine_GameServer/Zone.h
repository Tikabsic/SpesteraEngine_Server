#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include "TcpClient.h"

enum Zone_Name {
	MAIN = 7172
};

class Zone
{
public:
	Zone(boost::asio::io_context& io_context, unsigned short port, std::string address);
	~Zone();
	

private:

	Zone_Name get_zone_name_from_port(unsigned short port);

	boost::asio::io_context& io_context_;
	std::unique_ptr<TcpClient> tcp_client_;
	unsigned short port_;
	Zone_Name zone_;
};

