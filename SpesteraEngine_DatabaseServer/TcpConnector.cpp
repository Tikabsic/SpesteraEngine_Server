#include "TcpConnector.h"

TcpConnector::TcpConnector(boost::asio::io_context& io_context, const std::string& address, unsigned short port, std::string dburi)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(address), port)) {
    std::cout << "Tcp server on." << std::endl;
    database_uri = dburi;
    start_accept();
}

void TcpConnector::start_accept() {
    acceptor_.async_accept(
        [this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket) {
            if (!error) {
                auto new_connection = std::make_shared<ExternalConnection>(std::move(socket), database_uri);
                new_connection->start();
                external_connections_.push_back(std::move(new_connection));
                start_accept();
            }
        });
}