#include "DbServicesProvider.h"

std::unique_ptr<DbServicesProvider> DbServicesProvider::singleton_instance_ = nullptr;

DbServicesProvider::DbServicesProvider(boost::asio::io_context& io_context, const std::string& address, unsigned short port)
    : io_context_(io_context), address_(address), port_(port)
{
    std::cout << "DbServiceProvider instantiated" << std::endl;
    add_new_client();
}

DbServicesProvider& DbServicesProvider::instantiate(boost::asio::io_context& io_context, const std::string& address, unsigned short port) {
    if ( !singleton_instance_ ) {
        singleton_instance_ = std::unique_ptr<DbServicesProvider>(new DbServicesProvider(io_context, address, port));
    }
    return *singleton_instance_;
}

DbServicesProvider& DbServicesProvider::get_instance()
{
    if (!singleton_instance_) {
        std::cerr << "DbServiceProvider is not initialized!" << std::endl;
    }
    return *singleton_instance_;
}

void DbServicesProvider::send_query(const std::string& query) {
    std::lock_guard<std::mutex> lock(clients_mutex_);

    if ( db_clients_.empty() ) {
        add_new_client();
    }

    db_clients_[next_client_index_]->send(query);
    next_client_index_ = ( next_client_index_ + 1 ) % db_clients_.size();
}

void DbServicesProvider::add_new_client() {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    db_clients_.emplace_back(std::make_unique<TcpClient>(io_context_, address_, port_));
}

void DbServicesProvider::check_and_add_client(size_t connections_count) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    if ( db_clients_.size() < ( connections_count / balancing_threshold_ ) + 1 ) {
        add_new_client();
    }
}