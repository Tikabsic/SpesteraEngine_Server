#pragma once

#include <boost/asio.hpp>

#include "Zone.h"

class ZonesManager
{
public:
	ZonesManager(boost::asio::io_context& context);
	~ZonesManager();

private:
	boost::asio::io_context& io_context_;

	std::vector<std::unique_ptr<Zone>> zones_;
};

