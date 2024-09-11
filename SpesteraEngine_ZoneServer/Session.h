#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <deque>
#include <mutex>
#include <chrono>

#include "ZSProtocol.pb.h"
#include "BinaryCompressor.h"
#include "ZoneCharacter.h"

class TcpServer;
class ZoneMap;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver, ZoneMap* zonemap);
    ~Session();
    void start();
    void disconnect();

    void compress_to_write(const ZSWrapper& msg);
    void direct_push_to_buffer(const std::string& msg);

    void set_player_id(u_short pid);
    u_short get_player_id();
    std::shared_ptr<ZoneCharacter>& get_zone_character();

    //Updates
    void send_chunk_data(CellKey key);
    std::chrono::steady_clock timer_;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
private:

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    void do_read();
    void do_write();
    void handle_message(const ZSWrapper& wrapper);

    //callbacks
    void initialize_character();

    boost::asio::ip::tcp::socket socket_;
    int id_;
    enum { max_length = 512 * 1024};
    char data_[max_length];
    std::deque<std::string> write_msgs_;

    //Modules
    TcpServer* tcp_server_;
    ZoneMap* zone_map_;

    //Player things
    uint16_t playerId_;
    std::shared_ptr<ZoneCharacter> character_;
};

#endif // SESSION_H