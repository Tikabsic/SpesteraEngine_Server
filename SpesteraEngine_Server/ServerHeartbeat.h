#ifndef SERVERHEARTBEAT_H
#define SERVERHEARTBEAT_H

#include <boost/asio.hpp>
#include "UdpServer.h"

class ServerHeartbeat {
public:
    ServerHeartbeat(boost::asio::io_context& io_context, UdpServer& udpserver, int tickrate);

private:
    void do_heartbeat();

    UdpServer& server_;
    boost::asio::steady_timer timer_;
    int tickrate_;
};

#endif // SERVERHEARTBEAT_H