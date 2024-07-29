#pragma once

#include <memory>
#include <boost/asio.hpp>


class External_Connection : public std::enable_shared_from_this<External_Connection> {
public:
	External_Connection(boost::asio::ip::tcp::socket socket);

private:

	boost::asio::ip::tcp::socket socket_;
};

