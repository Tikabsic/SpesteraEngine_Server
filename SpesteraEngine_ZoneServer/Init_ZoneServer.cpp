#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "ConfigReader.h"
#include "UdpServer.h"
#include "TcpConnector.h"
#include "NetworkProtocol.pb.h"
#include "ZoneMap.h"
#include "HeartbeatsManager.h"

int main() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	try {
		// Read server configuration
		ConfigReader config("Config.ini");
		std::string address = config.get("Server", "ip");
		int tcp_port = std::stoi(config.get("Server", "tcp_port"));
		int tickrate = std::stoi(config.get("Heartbeat", "tickrate"));
		int udp_port = std::stoi(config.get("Server", "zone_udp_port"));

		// Create asio IO context
		boost::asio::io_context io_context;

		// Init modules
		std::unique_ptr<ZoneMap> map = std::make_unique<ZoneMap>(50);
		auto tcp_connector = std::make_unique<TcpConnector>(io_context, address, tcp_port);
		auto udp_server = std::make_unique<UdpServer>(io_context, address, udp_port);
		std::unique_ptr<HeartbeatsManager> heartbeat_manager = std::make_unique<HeartbeatsManager>(udp_server.get(), map.get());

		std::cout << "Zone Server loaded" << std::endl;

		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}

	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}