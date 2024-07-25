#include "TcpServer.h"
#include "ConnectionsManager.h"

TcpServer::TcpServer(boost::asio::io_context& io_context, const std::string& address, int port, ServerHeartbeat& server_heartbeat, ConnectionsManager* connmanager)
    : acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address(address), port)),
    next_id_(1),
    server_heartbeat_(server_heartbeat),
    conn_manager_(connmanager){
    do_accept();
}

void TcpServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket), next_id_, server_heartbeat_, this);
                session->set_player_id(next_id_);
                conn_manager_->create_new_connection(session->get_player_id(), session);
                session->start();
                ++next_id_;
            }
            do_accept();
        });
}

void TcpServer::deliver_to_all(const Wrapper& msg) {
    for (auto& session : conn_manager_->connections_) {
        session.second->tcp_connection_->compress_to_write(msg);
    }
}

void TcpServer::deliver_to_player(const u_short& player_id, const Wrapper& msg) {
    auto it = conn_manager_->get_connection(player_id);
    if (it != nullptr) {
        it->tcp_connection_->compress_to_write(msg);
    }
}

void TcpServer::remove_session(std::shared_ptr<Session> session) {
    conn_manager_->connections_.erase(session->get_player_id());
}

void TcpServer::deliver_to_other_direct(const std::string& msg, short session_id)
{
    for (auto other : conn_manager_->connections_) {
        if (other.second->tcp_connection_->get_player_id() == session_id)
            continue;
        if(other.second != nullptr)
            other.second->tcp_connection_->direct_push_to_buffer(msg);
    }
}
