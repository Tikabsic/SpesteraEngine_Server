#include "SpesteraConnection.h"

SpesteraConnection::SpesteraConnection(std::shared_ptr<Session> session)
    : tcp_connection_(session)
{
}

SpesteraConnection::~SpesteraConnection()
{
}

void SpesteraConnection::add_endpoint(udp::endpoint endpoint)
{
    udp_connection_ = endpoint;
}