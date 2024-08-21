#include "Session.h"
#include "TcpServer.h"
#include "ZoneMap.h"

#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket, int id, TcpServer* tcpserver, ZoneMap* zonemap)
    : socket_(std::move(socket)), id_(id), tcp_server_(tcpserver), zone_map_(zonemap) {
}

Session::~Session() {
    std::cout << "session closed" << std::endl;
}

void Session::start() {
    //Gathering initial player position, currently hardcoded, latelly will read from DB
    PlayerInitialData player_initial_data;
    player_initial_data.set_player_id(playerId_);
    player_initial_data.set_player_movementspeed(5);
    player_initial_data.set_position_x(1);
    player_initial_data.set_position_y(10);
    player_initial_data.set_position_z(1);


    ZSWrapper wrapper;
    wrapper.set_type(ZSWrapper::PLAYERINITIALDATA);
    wrapper.set_payload(player_initial_data.SerializeAsString());


    //pushing PlayerInitialData to buffer and senting it direct to client 
    write_msgs_.push_back(wrapper.SerializeAsString());
    do_write();

    //Creating data for other players to send and notice that new player logged in to game.
    PlayerIn new_client_data;
    new_client_data.mutable_data()->CopyFrom(player_initial_data);
    ZSWrapper client_data_wrapper;
    client_data_wrapper.set_type(ZSWrapper::PLAYERIN);
    client_data_wrapper.set_payload(new_client_data.SerializeAsString());
    tcp_server_->deliver_to_other_direct(client_data_wrapper.SerializeAsString(), playerId_);

    //Creating and pushing to ServerHeart shared_ptr of a Player_Character class wich represents and hold data of game character
    character_ = std::make_shared<ZoneCharacter>(*zone_map_, player_initial_data.position_x(), player_initial_data.position_y(), player_initial_data.position_z(), 5, playerId_, weak_from_this());
    zone_map_->spawn_movable_object_on_map(character_);

    do_read();
}

void Session::disconnect()
{
    std::cout << " Processing logout" << std::endl;
    tcp_server_->remove_session(shared_from_this());
    zone_map_->remove_session(shared_from_this());
    tcp_server_ = nullptr;
    zone_map_ = nullptr;
    socket_.close();
}

void Session::compress_to_write(const ZSWrapper& msg) {
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
    if (write_msgs_.empty()) {
        write_msgs_.push_back(msg);
        do_write();
    }
    else {
        write_msgs_.push_back(msg);
    }
}

void Session::set_player_id(u_short pid)
{
    playerId_ = pid;
}

void Session::handle_message(const ZSWrapper& wrapper) {
    switch (wrapper.type()) {
    case ZSWrapper::PLAYEROUT: {
        disconnect();
        break;
    }
    case ZSWrapper::PLAYERPOSITION: {
        PlayerPosition position;
        if (position.ParseFromArray(wrapper.payload().data(), wrapper.payload().size())) {
            character_->move_zone_character(position);
        }
        break;
    }
    default:
        std::cout << "Unknown message type received: " << wrapper.type() << std::endl;
        break;
    }
}

u_short Session::get_player_id()
{
    return playerId_;
}

std::shared_ptr<ZoneCharacter>& Session::get_zone_character()
{
    return character_;
}

void Session::do_read() {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                try{
                    ZSWrapper wrapper;
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
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [this](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                write_msgs_.pop_front();
                if (!write_msgs_.empty()) {
                    do_write();
                }
            }
        });
}