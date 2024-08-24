#include "HeartbeatsManager.h"
#include "ConfigReader.h"

HeartbeatsManager::HeartbeatsManager(TcpServer* tcpserver, ZoneMap* zonemap, boost::asio::io_context& iocontext) :
	tcp_server_(tcpserver), map_(zonemap), io_context_(iocontext)
{
	ConfigReader reader("Config.ini");
	int tickrate = std::stoi(reader.get("Heartbeat", "tickrate"));

	std::vector<Chunk*> chunks = map_->get_zonemap_chunks();

	for (auto chunk : chunks) {
		std::unique_ptr<ChunkHeartbeat> chunk_heartbeat = std::make_unique<ChunkHeartbeat>(io_context_, tickrate, chunk->get_sessions(), chunk->chunk_position_);
		chunk->set_chunk_heartbeat(chunk_heartbeat.get());
		heartbeats_.emplace_back(std::move(chunk_heartbeat));
	}

	std::cout << "Heartbeat manager initialized..." << std::endl;
}

HeartbeatsManager::~HeartbeatsManager()
{
}

