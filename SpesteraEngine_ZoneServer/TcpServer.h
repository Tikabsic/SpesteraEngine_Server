#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Session.h"

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

class ConnectionsManager;

using boost::asio::ip::tcp;

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, const std::string& address, int port, ConnectionsManager* connmanager, ZoneMap* zonemap);

    //Deliver Type
    void deliver_to_all(const ZSWrapper& msg);
    void deliver_to_player(const u_short& player_id, const ZSWrapper& msg);
    void deliver_to_other_direct(const std::string& msg, short session_id);

    //Session Management
    void add_session(std::shared_ptr<Session> session);
    void remove_session(std::shared_ptr<Session> session);

private:
    void do_accept();

    tcp::acceptor acceptor_;

    ConnectionsManager* conn_manager_;
    ZoneMap* zone_map_;

    u_short next_id_;
};

#endif // TCPSERVER_H