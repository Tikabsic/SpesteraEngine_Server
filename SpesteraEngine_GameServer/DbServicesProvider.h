#ifndef DBSERVICESPROVIDER_H
#define DBSERVICESPROVIDER_H

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <mutex>
#include "TcpClient.h"

class DbServicesProvider {
public:
    static DbServicesProvider& instantiate(boost::asio::io_context& io_context, const std::string& address, unsigned short port);
    static DbServicesProvider& get_instance();
    void send_query(const std::string& query);
    void check_and_add_client(size_t connections_count);

private:
    DbServicesProvider(boost::asio::io_context& io_context, const std::string& address, unsigned short port);

    boost::asio::io_context& io_context_;
    std::string address_;
    unsigned short port_;

    std::vector<std::unique_ptr<TcpClient>> db_clients_;
    std::mutex clients_mutex_;

    size_t next_client_index_ = 0;
    size_t balancing_threshold_ = 20;

    static std::unique_ptr<DbServicesProvider> singleton_instance_;

    void add_new_client();
};

#endif // DBSERVICESPROVIDER_H