#pragma once
#include <boost/asio.hpp>
#include <memory>

class ZoneConnection : public std::enable_shared_from_this<ZoneConnection> {

public:

	ZoneConnection(boost::asio::ip::udp::endpoint endpoint);

	const boost::asio::ip::udp::endpoint& get_endpoint() const;
private:
	boost::asio::ip::udp::endpoint endpoint_;
};

