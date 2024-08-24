// ConnectionsManager.h
#ifndef CONNECTIONSMANAGER_H
#define CONNECTIONSMANAGER_H

#include <unordered_map>
#include <memory>
#include <mutex>
#include <boost/asio.hpp>

#include "Session.h"

class ConnectionsManager {

public:
    ConnectionsManager();

    void create_new_connection(short key, const std::shared_ptr<Session>& session);
    void delete_connection_from_map(short key);

    std::shared_ptr<Session> get_connection(short connectionid);

    std::unordered_map<short, std::shared_ptr<Session>> connections_;
    std::mutex connections_mutex_;

};

#endif // CONNECTIONSMANAGER_H

