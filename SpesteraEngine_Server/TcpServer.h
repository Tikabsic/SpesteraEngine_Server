#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <boost/asio.hpp>
#include <memory>
#include <set>
#include <unordered_map>


#include "Session.h"

using boost::asio::ip::tcp;

class TcpServer {
public:
    TcpServer(boost::asio::io_context& io_context, const std::string& address, int port);
    void deliver_to_all(const Wrapper& msg);
    void deliver_to_player(const std::string& player_id, const Wrapper& msg); // Nowa funkcja

private:
    void do_accept();

    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<Session>> sessions_;
    std::unordered_map<std::string, std::shared_ptr<Session>> session_map_; // Mapa sesji po identyfikatorach
    uint32_t next_id_; // ID dla nastêpnej sesji
};

#endif // TCPSERVER_H