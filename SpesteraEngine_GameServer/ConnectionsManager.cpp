#include "ConnectionsManager.h"
#include "DbServicesProvider.h"
#include <iostream>


ConnectionsManager::ConnectionsManager()
{

}

void ConnectionsManager::create_new_connection(short key, std::shared_ptr<Session> session)
{
    std::lock_guard<std::mutex> lock(connections_mutex_);
    std::shared_ptr<SpesteraConnection> new_connection = std::make_shared<SpesteraConnection>(session);
    connections_.insert({key, new_connection });
    DbServicesProvider::get_instance().check_and_add_client(connections_.size());
    std::cout << "New player connected with id : " << key << ". Total players online : " << connections_.size() << "!" << std::endl;
}

void ConnectionsManager::add_endpoint_to_map(short key, udp::endpoint endpoint)
{
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto it = connections_.find(key);
    if (it != connections_.end()) {
        it->second->add_endpoint(endpoint);
    }
}

void ConnectionsManager::delete_connection_from_map(short key)
{
    std::lock_guard<std::mutex> lock(connections_mutex_);
    auto it = connections_.find(key);
    if (it != connections_.end()) {
        connections_.erase(it);
        std::cout << "Player with id: " << key << " disconnected, online players: " << connections_.size() << "!" << std::endl;
    }
}

std::shared_ptr<SpesteraConnection> ConnectionsManager::get_connection(short connectionid)
{
    std::lock_guard<std::mutex> lock(connections_mutex_);

    auto it = connections_.find(connectionid);
    if (it != connections_.end()) {
        return it->second;
    }
    return nullptr;
}

