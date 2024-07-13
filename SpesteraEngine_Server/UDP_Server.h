#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include "Session.h"

using boost::asio::ip::udp;

class UDP_Server {
public:
    UDP_Server(boost::asio::io_context& io_context, const std::string& address, int port);
    void send_to_player(const std::string& player_id, const Wrapper& msg);
    void send_to_all_players(const Wrapper& msg);

private:
    void do_receive();

    udp::socket socket_;
    enum { max_length = 512*1024 };
    char data_[max_length];
};

#endif // UDPSERVER_H