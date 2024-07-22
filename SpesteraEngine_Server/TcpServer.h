#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <boost/asio.hpp>
#include <memory>
#include <set>
#include <unordered_map>


#include "Session.h"
#include "ServerHeartbeat.h"

using boost::asio::ip::tcp;

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, const std::string& address, int port, ServerHeartbeat& server_heartbeat);
    void deliver_to_all(const Wrapper& msg);
    void deliver_to_player(const u_short& player_id, const Wrapper& msg);

private:
    void do_accept();

    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<Session>> sessions_;
    std::unordered_map<u_short, std::shared_ptr<Session>> session_map_;

    ServerHeartbeat& server_heartbeat_;

    u_short next_id_;
};

#endif // TCPSERVER_H