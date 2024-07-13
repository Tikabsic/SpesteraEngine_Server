#include "TcpServer.h"

TcpServer::TcpServer(boost::asio::io_context& io_context, const std::string& address, int port)
    : acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address(address), port)),
    next_id_(1) { // Inicjalizacja ID sesji
    do_accept();
}

void TcpServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket), next_id_);
                sessions_.insert(session);
                session_map_[std::to_string(next_id_)] = session; // Dodanie sesji do mapy
                session->start();
                ++next_id_; // Inkrementacja ID dla nastêpnej sesji
            }
            do_accept();
        });
}

void TcpServer::deliver_to_all(const Wrapper& msg) {
    for (auto& session : sessions_) {
        session->compress_to_write(msg);
    }
}

void TcpServer::deliver_to_player(const std::string& player_id, const Wrapper& msg) {
    auto it = session_map_.find(player_id);
    if (it != session_map_.end()) {
        it->second->compress_to_write(msg);
    }
}