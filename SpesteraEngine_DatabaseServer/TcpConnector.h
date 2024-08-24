#ifndef TCP_CONNECTOR_H
#define TCP_CONNECTOR_H

#include <boost/asio.hpp>
#include <memory>
#include "ExternalConnection.h"
#include "DbConnection.h"

class TcpConnector {
public:
    TcpConnector(boost::asio::io_context& io_context, const std::string& address, unsigned short port, std::string dburi);

private:
    void start_accept();

    std::vector<std::shared_ptr<ExternalConnection>> external_connections_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::string database_uri = "";
};

#endif // TCP_CONNECTOR_H