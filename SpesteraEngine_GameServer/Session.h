#pragma once

#include <boost/asio.hpp>
#include <deque>
#include <mutex>
#include <chrono>

//Protobuf
#include "GSProtocol.pb.h"
#include "DatabaseProtocol.pb.h"
#include "GameCharacterService.pb.h"
#include "AccountService.pb.h"
#include "Connection.pb.h"

#include "BinaryCompressor.h"
#include "PlayerCharacter.h"

class TcpServer;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver);
    virtual ~Session() = default;
    void start();
    void disconnect();

    void compress_to_write(const GSWrapperResponse& msg);
    void direct_push_to_buffer(const std::string& msg);

    //DbDataProcessing
    virtual void process_database_data(DatabaseResponseWrapper& wrapper) = 0;

    u_short get_player_id();
protected:
    std::chrono::steady_clock timer_;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;

    void do_read();
    void do_write();
    virtual void handle_message(const GSWrapperRequest& wrapper) = 0;

    //DbHandlers
    virtual void handle_account_service_response(const AccountServiceResponseWrapper& wrapper) = 0;
    virtual void handle_gamecharacter_service_response(const GameCharacterServiceResponseWrapper& wrapper) = 0;

    boost::asio::ip::tcp::socket socket_;
    int id_;
    enum { max_length = 512 * 1024};
    char data_[max_length];
    std::deque<std::string> write_msgs_;

    //Modules
    TcpServer* tcp_server_;

    std::string account_name_ = "";
    std::shared_ptr<PlayerCharacter> character_;
};
