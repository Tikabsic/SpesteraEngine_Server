// ConnectionsManager.h
#ifndef CONNECTIONSMANAGER_H
#define CONNECTIONSMANAGER_H

#include <unordered_map>
#include <memory>
#include <mutex>
#include <boost/asio.hpp>
#include "Session.h"
#include "SpesteraConnection.h"

class ConnectionsManager {

public:
    void create_new_connection(short key, std::shared_ptr<Session> session);
    void add_endpoint_to_map(short key, udp::endpoint endpoint);
    void delete_connection_from_map(short key);

    std::shared_ptr<SpesteraConnection> get_connection(short connectionid);

    std::unordered_map<short, std::shared_ptr<SpesteraConnection>> connections_;
    std::mutex connections_mutex_;

private:

};

#endif // CONNECTIONSMANAGER_H

