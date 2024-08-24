#include "ConnectionsManager.h"
#include "DbServicesProvider.h"
#include <iostream>


ConnectionsManager::ConnectionsManager()
{

}

void ConnectionsManager::create_new_connection(short key, std::shared_ptr<Session> session)
{
    connections_.insert({key, session });
    DbServicesProvider::get_instance().check_and_add_client(connections_.size());
    std::cout << "New player connected with id : " << key << ". Total players online : " << connections_.size() << "!" << std::endl;
}

void ConnectionsManager::delete_connection_from_map(short key)
{
    auto it = connections_.find(key);
    if (it != connections_.end()) {
        connections_.erase(it);
        std::cout << "Player with id: " << key << " disconnected, online players: " << connections_.size() << "!" << std::endl;
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

void ConnectionsManager::set_account_online(std::string accountname)
{
    online_accounts_.insert(accountname);
}

void ConnectionsManager::set_account_offline(std::string accountname)
{
    auto it = online_accounts_.find(accountname);
    if ( it == online_accounts_.end() ) {
        std::cerr << "Account not found, error!" << std::endl;
        return;
    }
    online_accounts_.erase(accountname);
}

bool ConnectionsManager::is_account_online(std::string accountname)
{
    auto it = online_accounts_.find(accountname);
    if ( it == online_accounts_.end() ) {
        return false;
    }
    return true;
}

