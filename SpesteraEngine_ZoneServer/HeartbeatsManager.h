#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <unordered_set>

#include "ChunkHeartbeat.h"
#include "ServerStopwatch.h"
#include "UdpServer.h"
#include "ZoneMap.h"


class HeartbeatsManager
{
public:
	HeartbeatsManager(UdpServer* udpserver, ZoneMap* zonemap);
	~HeartbeatsManager();

	void start_heartbeating();
private:
	boost::asio::io_context io_context_;
	UdpServer* udp_server_;
	ZoneMap* map_;
	std::unique_ptr<ServerStopwatch> stopwatch_;
	std::vector<std::unique_ptr<ChunkHeartbeat>> heartbeats_;
};

