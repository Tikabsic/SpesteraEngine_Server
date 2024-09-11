#pragma once

#include "Session.h"
#include "ServerStopwatch.h"

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
    void deliver_to_all(const GSWrapperResponse& msg);
    void deliver_to_player(const u_short& player_id, const GSWrapperResponse& msg);
    void deliver_to_other_direct(const std::string& msg, short session_id);

    //Session Disconnect
    void remove_session(std::shared_ptr<Session> session);

    //Account managing
    void connect_account(std::string accountname);
    void disconnect_account(std::string accountname);
    bool is_account_online(std::string accountname);
private:
    void do_accept();

    tcp::acceptor acceptor_;

    ConnectionsManager* conn_manager_;

    u_short next_id_;
};
