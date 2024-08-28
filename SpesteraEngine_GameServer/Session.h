#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <deque>
#include <mutex>

#include "GSProtocol.pb.h"
#include "DatabaseProtocol.pb.h"
#include "GameCharacterService.pb.h"
#include "AccountService.pb.h"
#include "BinaryCompressor.h"
#include "PlayerCharacter.h"

class TcpServer;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver);
    ~Session();
    void start();
    void disconnect();

    void compress_to_write(const GSWrapperResponse& msg);
    void direct_push_to_buffer(const std::string& msg);

    //DbDataProcessing
    void process_database_data(DatabaseResponseWrapper& wrapper);

    void set_player_id(u_short pid);
    u_short get_player_id();
private:
    void do_read();
    void do_write();
    void handle_message(const GSWrapperRequest& wrapper);

    //Authentication
    void process_login_request(const RequestLogin& request);
    bool verify_login_request(const ResponseAccountData& data);

    void handle_account_service_response(const AccountServiceResponseWrapper& wrapper);
    void handle_gamecharacter_service_response(const GameCharacterServiceResponseWrapper& wrapper);

    boost::asio::ip::tcp::socket socket_;
    int id_;
    enum { max_length = 512 * 1024};
    char data_[max_length];
    std::deque<std::string> write_msgs_;

    //Modules
    TcpServer* tcp_server_;

    //Account properties
    bool is_authenticated = false;
    std::string account_name_ = "";

    //Player things
    uint16_t playerId_;
    std::shared_ptr<PlayerCharacter> character_;
};

#endif // SESSION_H