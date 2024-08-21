#include "TcpServer.h"
#include "ConnectionsManager.h"

TcpServer::TcpServer(boost::asio::io_context& io_context, const std::string& address, int port, ConnectionsManager* connmanager, ZoneMap* zonemap)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address(address), port)),
    zone_map_(zonemap),
    next_id_(1),
    conn_manager_(connmanager) {
    std::cout << "Started accepting..." << std::endl;
    do_accept();
}

void TcpServer::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::cout << "Accepting..." << std::endl;
                auto session = std::make_shared<Session>(std::move(socket), next_id_, this, zone_map_);
                session->set_player_id(next_id_);
                conn_manager_->create_new_connection(session->get_player_id(), session);
                session->start();
                ++next_id_;
            }
            do_accept();
        });
}

void TcpServer::deliver_to_all(const ZSWrapper& msg) {
    for (auto& session : conn_manager_->connections_) {
        session.second->compress_to_write(msg);
    }
}

void TcpServer::deliver_to_player(const u_short& player_id, const ZSWrapper& msg) {
    auto it = conn_manager_->get_connection(player_id);
    if (it != nullptr) {
        it->compress_to_write(msg);
    }
}

void TcpServer::add_session(std::shared_ptr<Session> session)
{
    conn_manager_->create_new_connection(session->get_player_id(), session);
}

void TcpServer::remove_session(std::shared_ptr<Session> session) {

    PlayerOut logout_msg;
    logout_msg.set_player_id(session->get_player_id());

    ZSWrapper logout_wrapper;
    logout_wrapper.set_type(ZSWrapper::PLAYEROUT);
    logout_wrapper.set_payload(logout_msg.SerializeAsString());

    deliver_to_other_direct(logout_wrapper.SerializeAsString(), session->get_player_id());

    conn_manager_->delete_connection_from_map(session->get_player_id());

}

void TcpServer::deliver_to_other_direct(const std::string& msg, short session_id)
{
    for (auto other : conn_manager_->connections_) {
        if (other.second->get_player_id() == session_id)
            continue;
        if(other.second != nullptr)
            other.second->direct_push_to_buffer(msg);
    }
}
