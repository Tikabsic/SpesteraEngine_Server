#pragma once

#include <boost/asio.hpp>


#include "External_Connection.h"

using boost::asio::ip::tcp;

class TCP_Connector
{
public:
    TCP_Connector(boost::asio::io_context& io_context, const std::string& address, int port);


private:
    void do_accept();

    tcp::acceptor acceptor_;
};

