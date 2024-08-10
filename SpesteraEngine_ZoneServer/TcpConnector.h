#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <iostream>

#include "ExternalConnection.h"

class TcpConnector {
public:
    TcpConnector(boost::asio::io_context& io_context, const std::string& address, unsigned short port);

private:
    void start_accept();

    std::vector<std::shared_ptr<ExternalConnection>> external_connections_;
    boost::asio::ip::tcp::acceptor acceptor_;
};