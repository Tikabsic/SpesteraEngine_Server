#include "TcpConnector.h"

TcpConnector::TcpConnector(boost::asio::io_context& io_context, const std::string& address, unsigned short port, DbConnection* db_connection)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(address), port)),
    db_connection_(db_connection) {
    std::cout << "Tcp server on." << std::endl;
    start_accept();
}

void TcpConnector::start_accept() {
    acceptor_.async_accept(
        [this](const boost::system::error_code& error, boost::asio::ip::tcp::socket socket) {
            if (!error) {
                auto new_connection = std::make_unique<ExternalConnection>(std::move(socket), db_connection_);
                new_connection->start();
                external_connections_.push_back(std::move(new_connection));
                start_accept();
            }
        });
}