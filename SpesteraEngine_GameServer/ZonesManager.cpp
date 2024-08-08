#include "ZonesManager.h"
#include "ConfigReader.h"

ZonesManager::ZonesManager(boost::asio::io_context& context) :
	io_context_(context)
{
	ConfigReader reader("Config.ini");
	std::string address = reader.get("Server", "ip");
	std::vector<short> ports = reader.getZonePorts();

	for ( auto& port : ports ) {
		std::unique_ptr<Zone> zone = std::make_unique<Zone>(io_context_, port, address);
		zones_.emplace_back(std::move(zone));
	};

	std::cout << "Zone manager created..." << std::endl;
}

ZonesManager::~ZonesManager()
{
}
