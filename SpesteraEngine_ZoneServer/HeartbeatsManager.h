#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <unordered_set>

#include "ChunkHeartbeat.h"
#include "ServerStopwatch.h"
#include "TcpServer.h"
#include "ZoneMap.h"


class HeartbeatsManager
{
public:
	HeartbeatsManager(TcpServer* tcpserver, ZoneMap* zonemap, boost::asio::io_context& iocontext);
	~HeartbeatsManager();

private:
	boost::asio::io_context& io_context_;
	TcpServer* tcp_server_;
	ZoneMap* map_;
	std::unique_ptr<ServerStopwatch> stopwatch_;
	std::vector<std::unique_ptr<ChunkHeartbeat>> heartbeats_;
};

