#include "TcpClient.h"
#include "DbServicesProvider.h"

TcpClient::TcpClient(boost::asio::io_context& io_context, const std::string& address, unsigned short port, Client_Type clienttype)
    : io_context_(io_context), resolver_(io_context), socket_(io_context), address_(address), port_(port), reconnect_timer_(io_context), type_(clienttype) {
    connect();
}

void TcpClient::connect() {
    tcp::resolver::query query(address_, std::to_string(port_));
    resolver_.async_resolve(query,
        [ this ] (const boost::system::error_code& ec, tcp::resolver::results_type endpoints) {
            if ( !ec ) {
                do_connect(endpoints);
            }
            else {
                std::cerr << "Resolve error: " << ec.message() << std::endl;
            }
        });
}

void TcpClient::do_connect(const tcp::resolver::results_type& endpoints) {
    boost::asio::async_connect(socket_, endpoints,
        [ this ] (const boost::system::error_code& ec, const tcp::endpoint& /*endpoint*/) {
        if ( !ec ) {
            std::cout << "TcpClient connection established!" << std::endl;
            do_read();
        }
        else {
            std::cerr << "Connect error: " << ec.message() << std::endl;
            reconnect_timer_.expires_after(std::chrono::seconds(1));
            reconnect_timer_.async_wait([ this ] (const boost::system::error_code& ec) {
                if ( !ec ) {
                    std::cout << "Reconnecting to: " << address_ << ":" << port_ << std::endl;
                    connect();
                }
            });
        }
        });
}

void TcpClient::send(const std::string& message) {
    std::lock_guard<std::mutex> lock(write_msgs_mutex_);
    write_msgs_.push_back(message);
    do_write();
}

void TcpClient::do_write() {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [this] (boost::system::error_code ec, std::size_t length) {
            if ( !ec ) {
                write_msgs_.pop_front();
                if ( !write_msgs_.empty() ) {
                    do_write();
                }
            }
        });
}

void TcpClient::do_read() {
    socket_.async_read_some(boost::asio::buffer(read_msg_, max_length),
        [this] (boost::system::error_code ec, std::size_t length) {
            if ( !ec ) {
                if ( length > 0 ) {
                    switch ( type_ ) {
                    case Client_Type::DB_SERVICE_PROVIDER:
                        DbServicesProvider::get_instance().process_response(std::string(read_msg_, length));
                        break;

                    case Client_Type::ZONE_SERVER_CONNECTION:

                        break;
                    }
                }
                do_read();
            }
        });
}
