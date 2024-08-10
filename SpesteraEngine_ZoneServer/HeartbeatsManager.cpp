#include "HeartbeatsManager.h"
#include "ConfigReader.h"

HeartbeatsManager::HeartbeatsManager(UdpServer* udpserver, ZoneMap* zonemap) :
	udp_server_(udpserver), map_(zonemap), io_context_()
{
	boost::asio::io_context context;
	ConfigReader reader("Config.ini");
	int tickrate = std::stoi(reader.get("Heartbeat", "tickrate"));

	std::vector<Chunk*> chunks = map_->get_zonemap_chunks();

	for (auto chunk : chunks) {
		std::unique_ptr<ChunkHeartbeat> chunk_heartbeat = std::make_unique<ChunkHeartbeat>(io_context_, *udp_server_, tickrate);
		chunk->set_chunk_heartbeat(chunk_heartbeat.get());
		heartbeats_.emplace_back(std::move(chunk_heartbeat));
	}

	start_heartbeating();
	std::cout << "Heartbeat manager initialized..." << std::endl;
	io_context_.run();
}

HeartbeatsManager::~HeartbeatsManager()
{
}

void HeartbeatsManager::start_heartbeating()
{
	for (auto& heartbeat : heartbeats_) {
		heartbeat->do_heartbeat();
	}
}
