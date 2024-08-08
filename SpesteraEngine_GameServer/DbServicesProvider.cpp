#include "DbServicesProvider.h"

std::unique_ptr<DbServicesProvider> DbServicesProvider::singleton_instance_ = nullptr;

DbServicesProvider::DbServicesProvider(boost::asio::io_context& io_context, const std::string& address, unsigned short port, ConnectionsManager& connmanager)
    : io_context_(io_context), address_(address), port_(port), conn_manager_(connmanager)
{
    std::cout << "DbServiceProvider instantiated" << std::endl;
    add_new_client();
}

DbServicesProvider& DbServicesProvider::instantiate(boost::asio::io_context& io_context, const std::string& address, unsigned short port, ConnectionsManager& connmanager) {
    if ( !singleton_instance_ ) {
        singleton_instance_ = std::unique_ptr<DbServicesProvider>(new DbServicesProvider(io_context, address, port, connmanager));
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

void DbServicesProvider::process_response(const std::string& response)
{
    try {
        ResponseWrapper wrapper;
        if ( wrapper.ParseFromString(response) ) {
            auto session = conn_manager_.get_connection(wrapper.response_id())->tcp_connection_;
            session->direct_push_to_buffer("siemano");
        }
    }
    catch ( const std::runtime_error& e ) {

    }
}

void DbServicesProvider::add_new_client() {
    db_clients_.emplace_back(std::make_unique<TcpClient>(io_context_, address_, port_, Client_Type::DB_SERVICE_PROVIDER));
}

void DbServicesProvider::check_and_add_client(u_short connections_count) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    if ( db_clients_.size() < ( connections_count / balancing_threshold_ ) + 1 ) {
        add_new_client();
    }
}