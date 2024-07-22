#include "TcpServer.h"

TcpServer::TcpServer(boost::asio::io_context& io_context, const std::string& address, int port, ServerHeartbeat& server_heartbeat)
    : acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address(address), port)),
    next_id_(1),
    server_heartbeat_(server_heartbeat){
    do_accept();
}

void TcpServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket), next_id_, server_heartbeat_, this);
                sessions_.push_back(session);
                session->set_player_id(next_id_);
                session_map_[session->get_player_id()] = session;
                session->start();
                ++next_id_;
            }
            do_accept();
        });
}

void TcpServer::deliver_to_all(const Wrapper& msg) {
    for (auto& session : sessions_) {
        session->compress_to_write(msg);
    }
}

void TcpServer::deliver_to_player(const u_short& player_id, const Wrapper& msg) {
    auto it = session_map_.find(player_id);
    if (it != session_map_.end()) {
        it->second->compress_to_write(msg);
    }
}

void TcpServer::remove_session(std::shared_ptr<Session> session) {
    sessions_.erase(std::remove(sessions_.begin(), sessions_.end(), session), sessions_.end());
    session_map_.erase(session->get_player_id());
}

void TcpServer::deliver_to_other(const Wrapper& msg, short session_id)
{

    for (auto other : sessions_) {
        if (other->get_player_id() == session_id)
            continue;
        if(other != nullptr)
            other->compress_to_write(msg);
    }
}
