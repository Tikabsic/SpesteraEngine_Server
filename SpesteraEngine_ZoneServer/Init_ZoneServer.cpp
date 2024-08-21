#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "ConfigReader.h"
#include "TcpServer.h"
#include "TcpConnector.h"
#include "ZSProtocol.pb.h"
#include "ZoneMap.h"
#include "HeartbeatsManager.h"
#include "ConnectionsManager.h"

int main() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	try {
		// Read server configuration
		ConfigReader config("Config.ini");
		std::string address = config.get("Server", "ip");
		int connector_port = std::stoi(config.get("Server", "connector_port"));
		int tickrate = std::stoi(config.get("Heartbeat", "tickrate"));
		int zone_port = std::stoi(config.get("Server", "zone_tcp_port"));

		// Create asio IO context
		boost::asio::io_context io_context;

		// Init modules
		std::unique_ptr<ConnectionsManager> conn_manager = std::make_unique<ConnectionsManager>();
		std::unique_ptr<ZoneMap> map = std::make_unique<ZoneMap>(50);
		auto tcp_connector = std::make_unique<TcpConnector>(io_context, address, connector_port);
		auto tcp_server = std::make_unique<TcpServer>(io_context, address, zone_port, conn_manager.get(), map.get());
		std::unique_ptr<HeartbeatsManager> heartbeat_manager = std::make_unique<HeartbeatsManager>(tcp_server.get(), map.get(), io_context);

		std::cout << "Zone Server loaded" << std::endl;

		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}