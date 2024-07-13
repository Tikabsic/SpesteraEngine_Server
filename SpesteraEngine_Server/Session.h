#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <deque>
#include <mutex>
#include "NetworkProtocol.pb.h"
#include "BinaryCompressor.h"

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, int id);
    void start();
    void compress_to_write(const Wrapper& msg);

private:
    void do_read();
    void do_write();
    std::string GenerateSessionID();
    void handle_message(const Wrapper& wrapper);

    boost::asio::ip::tcp::socket socket_;
    int id_;
    enum { max_length = 512 * 1024};
    char data_[max_length];
    std::deque<std::string> write_msgs_;
    std::string playerId_;
    std::mutex write_mutex_;
};

#endif // SESSION_H