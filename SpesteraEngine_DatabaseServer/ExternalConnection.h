#ifndef EXTERNAL_CONNECTION_H
#define EXTERNAL_CONNECTION_H

#include <boost/asio.hpp>
#include <memory>
#include "DbConnection.h"

class ExternalConnection : public std::enable_shared_from_this<ExternalConnection> {
public:
    ExternalConnection(boost::asio::ip::tcp::socket socket, DbConnection* db_connection);

    boost::asio::ip::tcp::socket& socket();
    void start();

private:
    void read_data();
    void send_data();

    //Base connection fields
    boost::asio::ip::tcp::socket socket_;
    DbConnection* db_connection_;
    enum { max_length = 4 * 1024 };
    char recive_data_[max_length];
    std::deque<std::string> send_data_;
};

#endif // EXTERNAL_CONNECTION_H