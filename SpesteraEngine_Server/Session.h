#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <deque>
#include <mutex>

#include "NetworkProtocol.pb.h"
#include "BinaryCompressor.h"
#include "Player_Character.h"
#include "ServerHeartbeat.h"

class TcpServer;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, int id, ServerHeartbeat& heartbeat, TcpServer* tcpserver);
    void start();

    void compress_to_write(const Wrapper& msg);
    void direct_push_to_buffer(const std::string& msg);

    void set_player_id(u_short pid);
    u_short get_player_id();
private:
    void do_read();
    void do_write();
    void handle_message(const Wrapper& wrapper);

    boost::asio::ip::tcp::socket socket_;
    int id_;
    enum { max_length = 512 * 1024};
    char data_[max_length];
    std::deque<std::string> write_msgs_;
    std::mutex write_mutex_;

    //Modules
    ServerHeartbeat& server_heartbeat_;
    TcpServer* tcp_server_;

    //Player things
    uint16_t playerId_;
    std::shared_ptr<Player_Character> character_;
};

#endif // SESSION_H