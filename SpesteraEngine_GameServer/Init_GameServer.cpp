#include <boost/asio.hpp>
#include <iostream>
#include <memory>

#include "ConfigReader.h"
#include "ConnectionsManager.h"
#include "TcpServer.h"
#include "UdpServer.h"
#include "ServerHeartbeat.h"
#include "NetworkProtocol.pb.h"
#include "BinaryCompressor.h"
#include "DbServicesProvider.h"

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    try {
        // Read server configuration
        ConfigReader config("Config.ini");
        std::string address = config.get("Server", "ip");
        int tcp_port = std::stoi(config.get("Server", "tcp_port"));
        int tickrate = std::stoi(config.get("Heartbeat", "tickrate"));
        int udp_port = std::stoi(config.get("Server", "udp_port"));
        int db_port = std::stoi(config.get("DatabaseServer", "db_port"));

        // Create asio IO context
        boost::asio::io_context io_context;

        // Init modules
        auto& db_services_provider = DbServicesProvider::instantiate(io_context, address, db_port);
        auto conn_manager = std::make_unique<ConnectionsManager>(&db_services_provider);
        auto udp_server = std::make_unique<UdpServer>(io_context, address, udp_port, conn_manager.get());
        auto heartbeat = std::make_unique<ServerHeartbeat>(io_context, *udp_server, tickrate);
        auto tcp_server = std::make_unique<TcpServer>(io_context, address, tcp_port, *heartbeat, conn_manager.get());

        std::cout << "Game Server loaded" << std::endl;

        io_context.run();
    }
    catch ( std::exception& e ) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}