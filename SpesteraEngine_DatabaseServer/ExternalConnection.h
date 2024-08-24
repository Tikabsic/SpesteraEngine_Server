#pragma once

#include <boost/asio.hpp>
#include <memory>
#include "DbConnection.h"
#include "AccountService.h"

class ExternalConnection : public std::enable_shared_from_this<ExternalConnection> {
public:
    ExternalConnection(boost::asio::ip::tcp::socket socket, std::string dburi);

    boost::asio::ip::tcp::socket& socket();
    void start();

private:
    void read_data();
    void send_data();

    void handle_message(const RequestWrapper& wrapper);

    void push_and_send(std::string message);

    //Base connection fields
    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 4 * 1024 };
    char recive_data_[max_length];
    std::deque<std::string> send_data_;

    //Services
    AccountService* account_service_;
};
