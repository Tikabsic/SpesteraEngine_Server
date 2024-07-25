#pragma once
#include "Session.h"
#include <memory>
#include <boost/asio.hpp>

class SpesteraConnection : public std::enable_shared_from_this<SpesteraConnection>
{
public:
    boost::asio::ip::udp::endpoint udp_connection_;
    std::shared_ptr<Session> tcp_connection_;

    SpesteraConnection(std::shared_ptr<Session> session);
    ~SpesteraConnection();
    void add_endpoint(udp::endpoint endpoint);
};

