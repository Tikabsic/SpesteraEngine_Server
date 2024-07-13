#include <boost/asio.hpp>
#include <iostream>
#include "ConfigReader.h"
#include "TcpServer.h"
#include "UdpServer.h"
#include "ServerHeartbeat.h"
#include "NetworkProtocol.pb.h"

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    try {
        //read server configuration

        ConfigReader config("Config.ini");

        std::string address = config.get("Server", "ip");
        int tcp_port = std::stoi(config.get("Server", "tcp_port"));
        int tickrate = std::stoi(config.get("Heartbeat", "tickrate"));
        int udp_port = std::stoi(config.get("Server", "udp_port"));

        //create asio IO context

        boost::asio::io_context io_context;

        //Init modules

        TcpServer tcp_server(io_context, address, tcp_port);
        UdpServer udp_server(io_context, address, udp_port);
        ServerHeartbeat heartbeat(io_context, udp_server, tickrate);

        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}