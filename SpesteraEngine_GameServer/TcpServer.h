#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Session.h"
#include "ServerHeartbeat.h"


#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

class ConnectionsManager;

using boost::asio::ip::tcp;

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, const std::string& address, int port, ConnectionsManager* connmanager);

    //Deliver Type
    void deliver_to_all(const Wrapper& msg);
    void deliver_to_player(const u_short& player_id, const Wrapper& msg);
    void deliver_to_other_direct(const std::string& msg, short session_id);

    //Session Disconnect
    void remove_session(std::shared_ptr<Session> session);

private:
    void do_accept();

    tcp::acceptor acceptor_;

    ConnectionsManager* conn_manager_;

    u_short next_id_;
};

#endif // TCPSERVER_H