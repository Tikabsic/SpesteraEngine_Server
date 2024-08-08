#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>

#include "ServerHeartbeat.h"
#include "ServerStopwatch.h"

class HeartbeatsManager
{
public:
	HeartbeatsManager(boost::asio::io_context& ioconext);
	~HeartbeatsManager();

	void start_heartbeating();
private:
	std::unique_ptr<ServerStopwatch> stopwatch_;
	std::vector<std::unique_ptr<ServerHeartbeat>> heartbeats_;
};

