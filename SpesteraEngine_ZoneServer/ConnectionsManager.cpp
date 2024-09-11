#include "ConnectionsManager.h"
#include <iostream>


ConnectionsManager::ConnectionsManager()
{

}

void ConnectionsManager::create_new_connection(short key, const std::shared_ptr<Session>& session)
{
    connections_.insert({key, session });
    std::cout << "New player connected with id : " << key << ". Total players online : " << connections_.size() << "!" << std::endl;
}

void ConnectionsManager::delete_connection_from_map(short key)
{
    auto it = connections_.find(key);
    if (it != connections_.end()) {
        connections_.erase(it);
        std::cout << "Player with id: " << key << " disconnected, online players in zone: " << connections_.size() << "!" << std::endl;
    }
}

std::shared_ptr<Session> ConnectionsManager::get_connection(short connectionid)
{
    auto it = connections_.find(connectionid);
    if (it != connections_.end()) {
        return it->second;
    }
    return nullptr;
}

