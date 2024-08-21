#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "ConfigReader.h"
#include "ConnectionsManager.h"
#include "TcpServer.h"
#include "ZonesManager.h"
#include "GSProtocol.pb.h"
#include "BinaryCompressor.h"
#include "DbServicesProvider.h"

int main() {

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    try {
        // Read server configuration
        ConfigReader config("Config.ini");
        std::string address = config.get("Server", "ip");
        int tcp_port = std::stoi(config.get("Server", "tcp_port"));
        int db_port = std::stoi(config.get("DatabaseServer", "db_port"));

        // Create asio IO contexts
        boost::asio::io_context io_context;

        // Init modules
        auto zones_manager = std::make_unique<ZonesManager>(io_context);
        auto conn_manager = std::make_unique<ConnectionsManager>();
        auto& db_services_provider = DbServicesProvider::instantiate(io_context, address, db_port, *conn_manager);
        auto tcp_server = std::make_unique<TcpServer>(io_context, address, tcp_port, conn_manager.get());

        std::cout << "Game Server loaded" << std::endl;

        // Create a thread to run the db_io_context
        io_context.run();
    }
    catch ( std::exception& e ) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}