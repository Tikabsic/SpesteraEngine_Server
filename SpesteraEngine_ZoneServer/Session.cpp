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
    do_read();
}

void Session::disconnect()
{
    socket_.cancel();
    tcp_server_->send_logout_message(shared_from_this());
    socket_.close();

    if (zone_map_) {
        zone_map_->remove_session(shared_from_this());
        zone_map_ = nullptr;
    }

    if (character_) {
        character_->delete_session_ptr();
        character_.reset();
    }

    if (tcp_server_) {
        tcp_server_->send_logout_message(shared_from_this());
        tcp_server_->remove_session(id_);
        tcp_server_ = nullptr;
    }
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
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    
    if (!write_in_progress) {
        do_write();
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
    case ZSWrapper::REQUESTWORLDDATA: {
        zone_map_->spawn_movable_object_on_map(character_);
        WorldData world_data = zone_map_->grab_world_data(character_);

        ZSWrapper wrapper;
        wrapper.set_type(ZSWrapper::WORLDDATA);
        wrapper.set_payload(world_data.SerializeAsString());
        compress_to_write(wrapper);
        break;
    }
    case ZSWrapper::ASSIGNCHARACTER: {
        initialize_character();
        break;
    }
    default:
        std::cout << "Unknown message type received: " << wrapper.type() << std::endl;
        break;
    }
}

void Session::initialize_character()
{
    //Gathering initial player position, currently hardcoded, latelly will read from DB
    PlayerInitialData player_initial_data;
    player_initial_data.set_player_id(playerId_);
    player_initial_data.set_player_movementspeed(5);
    player_initial_data.set_position_x(240);
    player_initial_data.set_position_y(10);
    player_initial_data.set_position_z(240);


    ZSWrapper wrapper;
    wrapper.set_type(ZSWrapper::PLAYERINITIALDATA);
    wrapper.set_payload(player_initial_data.SerializeAsString());


    //pushing PlayerInitialData to buffer and senting it direct to client 
    direct_push_to_buffer(wrapper.SerializeAsString());

    //Creating data for other players to send and notice that new player logged in to game.
    PlayerIn new_client_data;
    new_client_data.mutable_data()->CopyFrom(player_initial_data);
    ZSWrapper client_data_wrapper;
    client_data_wrapper.set_type(ZSWrapper::PLAYERIN);
    client_data_wrapper.set_payload(new_client_data.SerializeAsString());
    tcp_server_->deliver_to_other_direct(client_data_wrapper.SerializeAsString(), playerId_);

    //Creating and pushing to ServerHeart shared_ptr of a Player_Character class wich represents and hold data of game character
    character_ = std::make_shared<ZoneCharacter>(*zone_map_, player_initial_data.position_x(), player_initial_data.position_y(), player_initial_data.position_z(), 5, playerId_, weak_from_this());
}

u_short Session::get_player_id()
{
    return playerId_;
}

std::shared_ptr<ZoneCharacter>& Session::get_zone_character()
{
    return character_;
}

void Session::send_chunk_data(CellKey key)
{
    zone_map_->grab_chunk_data(key, playerId_);
    ZSWrapper wrapper;
    wrapper.set_type(ZSWrapper::WORLDDATA);
    wrapper.set_payload(zone_map_->grab_chunk_data(key, playerId_).SerializeAsString());

    compress_to_write(wrapper);
}

S

void Session::do_write() {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                self->write_msgs_.pop_front();
                if (!self->write_msgs_.empty()) {
                    self->do_write();
                }
            }
        });
}