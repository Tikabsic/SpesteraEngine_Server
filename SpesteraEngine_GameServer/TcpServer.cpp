#include "TcpServer.h"
#include "ConnectionsManager.h"

TcpServer::TcpServer(boost::asio::io_context& io_context, const std::string& address, int port, ConnectionsManager* connmanager)
    : acceptor_(io_context, tcp::endpoint(boost::asio::ip::make_address(address), port)),
    next_id_(1),
    conn_manager_(connmanager){
    do_accept();
}

void TcpServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                auto session = std::make_shared<Session>(std::move(socket), next_id_, this);
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

    ClientLogout logout_msg;
    logout_msg.set_player_id(session->get_player_id());

    Wrapper logout_wrapper;
    logout_wrapper.set_type(Wrapper::CLIENTLOGOUT);
    logout_wrapper.set_payload(logout_msg.SerializeAsString());

    deliver_to_other_direct(logout_wrapper.SerializeAsString(), session->get_player_id());

    conn_manager_->delete_connection_from_map(session->get_player_id());

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
