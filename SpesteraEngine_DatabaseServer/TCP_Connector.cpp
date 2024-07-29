#include "TCP_Connector.h"

TCP_Connector::TCP_Connector(boost::asio::io_context& io_context, const std::string& address, int port) :
    acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address(address), port))
{

}

void TCP_Connector::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto external_connection = std::make_shared<External_Connection>(std::move(socket));
            }
            do_accept();
        });
}
