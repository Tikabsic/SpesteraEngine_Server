#include "Session.h"
#include "TcpServer.h"
#include "DbServicesProvider.h"

#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver)
    : socket_(std::move(socket)), id_(id), tcp_server_(tcpserver) {
}

Session::~Session() {
    std::cout << "session closed" << std::endl;
}

void Session::start() {

    //Gathering initial player position, currently hardcoded, latelly will read from DB
    PlayerInitialData player_initial_data;
    player_initial_data.set_player_id(playerId_);
    player_initial_data.set_player_movementspeed(5);
    player_initial_data.set_position_x(100);
    player_initial_data.set_position_y(10);
    player_initial_data.set_position_z(100);

    Wrapper wrapper;
    wrapper.set_type(Wrapper::PLAYERINITIALDATA);
    wrapper.set_payload(player_initial_data.SerializeAsString());

    //Creating data for other players to send and notice that new player logged in to game.
    ClientLogin new_client_data;
    new_client_data.mutable_player_data()->CopyFrom(player_initial_data);
    Wrapper client_data_wrapper;
    client_data_wrapper.set_type(Wrapper::CLIENTLOGIN);
    client_data_wrapper.set_payload(new_client_data.SerializeAsString());
    tcp_server_->deliver_to_other_direct(client_data_wrapper.SerializeAsString(), playerId_);

    //Creating and pushing to ServerHeart shared_ptr of a Player_Character class wich represents and hold data of game character
    character_ = std::make_unique<PlayerCharacter>(player_initial_data.position_x(), player_initial_data.position_y(), player_initial_data.position_z(), playerId_);

    //pushing PlayerInitialData to buffer and senting it direct to client 
    write_msgs_.push_back(wrapper.SerializeAsString());
    do_write();
    do_read();
}

void Session::disconnect()
{

}

void Session::compress_to_write(const Wrapper& msg) {
    bool write_in_progress = !write_msgs_.empty();
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(msg.SerializeAsString(), compressed_msg);
    write_msgs_.push_back(compressed_msg);
    if (!write_in_progress) {
        do_write();
    }
}

void Session::direct_push_to_buffer(const std::string& msg)
{
    write_msgs_.push_back(msg);
    do_write();
}

void Session::set_player_id(u_short pid)
{
    playerId_ = pid;
}



void Session::handle_message(const Wrapper& wrapper) {
    switch (wrapper.type()) {
    case Wrapper::CLIENTLOGOUT: {
        std::cout << " Processing logout" << std::endl;
        tcp_server_->remove_session(shared_from_this());
        socket_.close();
        break;
    }
    case Wrapper::PLAYERPOSITION: {
        PlayerPosition position;
        if (position.ParseFromArray(wrapper.payload().data(), wrapper.payload().size())) {
            //character_->move_player_character(position);
        }
        else {
            std::cerr << "Failed to parse PlayerPosition message" << std::endl;
        }
        break;
    }
    default:
        std::cout << "Unknown message type" << std::endl;
        break;
    }
}

u_short Session::get_player_id()
{
    return playerId_;
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                try{
                    Wrapper wrapper;
                    if (wrapper.ParseFromArray(data_, length)) {
                        handle_message(wrapper);
                    }
                    do_read();
                }
                catch (const std::runtime_error& e) {

                }
            }
        });
}

void Session::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                write_msgs_.pop_front();
                if (!write_msgs_.empty()) {
                    do_write();
                }
            }
        });
}