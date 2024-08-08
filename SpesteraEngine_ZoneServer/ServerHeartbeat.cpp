#include "ServerHeartbeat.h"
#include "NetworkProtocol.pb.h"
#include "BinaryCompressor.h"
#include "Player_Character.h"

#include <chrono>

ServerHeartbeat::ServerHeartbeat(boost::asio::io_context& io_context, UdpServer& udpserver, int tickrate)
    : server_(udpserver),
    timer_(io_context, boost::asio::chrono::milliseconds(tickrate)),
    tickrate_(tickrate),
    prev_timestamp(0)
{
    //do_heartbeat();
}

void ServerHeartbeat::push_player_character(std::shared_ptr<Player_Character> character)
{
    online_characters_.push_back(character);
}

void ServerHeartbeat::remove_player_character(std::shared_ptr<Player_Character> character) {
    std::lock_guard<std::mutex> lock(characters_mutex_);
    auto it = std::find(online_characters_.begin(), online_characters_.end(), character);
    if (it != online_characters_.end()) {
        online_characters_.erase(it);
    }
    else {
        std::cerr << "Character not found in online_characters_." << std::endl;
    }
}

Wrapper ServerHeartbeat::gather_initial_world_data(uint16_t playerId) {
    Wrapper initial_data_wrapper;

    if (online_characters_.empty()) {
        std::cout << "No online characters." << std::endl;
        return initial_data_wrapper;
    }

    WorldData initial_data;
    for (auto& player_data : online_characters_) {

        if (playerId == player_data->player_id_) {
            continue;
        }
        PlayerInitialData player_initial_data;
        player_initial_data.set_player_id(player_data->player_id_);
        player_initial_data.set_position_x(player_data->player_transform_.position.x);
        player_initial_data.set_position_y(player_data->player_transform_.position.y);
        player_initial_data.set_position_z(player_data->player_transform_.position.z);
        player_initial_data.set_rotation_y(player_data->player_transform_.rotation_Y);
        player_initial_data.set_player_movementspeed(player_data->get_player_movement_speed());

        initial_data.add_players()->CopyFrom(player_initial_data);
    }

    initial_data_wrapper.set_type(Wrapper::WORLDDATA);
    initial_data_wrapper.set_payload(initial_data.SerializeAsString());

    return initial_data_wrapper;
}

void ServerHeartbeat::do_heartbeat()
{
        if (online_characters_.size() < 1) {
            timer_.expires_after(boost::asio::chrono::milliseconds(tickrate_));
            timer_.async_wait([this](boost::system::error_code ec) {
                if (!ec) {
                    do_heartbeat();
                }
                });
            return;
        }

    Heartbeat chunkHeartbeat;

    for (auto player : online_characters_) {
        if (player->is_character_moving()) {
            auto position = ServerHeartbeat::gather_player_transformation(player, true);
            chunkHeartbeat.add_players()->CopyFrom(position);
        }
    }

    Wrapper wrapper;
    wrapper.set_type(Wrapper::HEARTBEAT);
    wrapper.set_payload(chunkHeartbeat.SerializeAsString());
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(wrapper.SerializeAsString(), compressed_msg);

    server_.send_data_to_all_players(compressed_msg);

    timer_.expires_after(boost::asio::chrono::milliseconds(tickrate_));
    timer_.async_wait([this](boost::system::error_code ec) {
        if (!ec) {
            do_heartbeat();
        }
        });
}

PlayerPosition ServerHeartbeat::gather_player_transformation(std::shared_ptr<Player_Character> character, bool is_heartbeat)
{
    PlayerPosition transform;

    transform.set_player_id(character->player_id_);
    transform.set_position_x(character->player_transform_.position.x);
    transform.set_position_y(character->player_transform_.position.y);
    transform.set_position_z(character->player_transform_.position.z);

    if (is_heartbeat) {
        character->set_last_sent_position();
    }

    return transform;
}