#include "ChunkHeartbeat.h"
#include "NetworkProtocol.pb.h"
#include "BinaryCompressor.h"

#include <chrono>

ChunkHeartbeat::ChunkHeartbeat(boost::asio::io_context& io_context, UdpServer& udpserver, int tickrate)
    : server_(udpserver),
    timer_(io_context, boost::asio::chrono::milliseconds(tickrate)),
    tickrate_(tickrate),
    prev_timestamp(0)
{
    //do_heartbeat();
}

void ChunkHeartbeat::push_zone_character(std::shared_ptr<ZoneCharacter> character)
{
    online_characters_.push_back(character);
}

void ChunkHeartbeat::remove_zone_character(std::shared_ptr<ZoneCharacter> character) {
    auto it = std::find(online_characters_.begin(), online_characters_.end(), character);
    if (it != online_characters_.end()) {
        online_characters_.erase(it);
    }
    else {
        std::cerr << "Character not found in online_characters_." << std::endl;
    }
}

Wrapper ChunkHeartbeat::gather_initial_world_data(uint16_t playerId) {
    Wrapper initial_data_wrapper;

    if (online_characters_.empty()) {
        std::cout << "No online characters." << std::endl;
        return initial_data_wrapper;
    }

    WorldData initial_data;
    for (auto& player_data : online_characters_) {

        if (playerId == player_data->get_player_id()) {
            continue;
        }
        PlayerInitialData player_initial_data;
        player_initial_data.set_player_id(player_data->get_player_id());
        player_initial_data.set_position_x(player_data->get_transform().x);
        player_initial_data.set_position_y(player_data->get_transform().y);
        player_initial_data.set_position_z(player_data->get_transform().z);
        player_initial_data.set_player_movementspeed(player_data->get_character_movement_speed());

        initial_data.add_players()->CopyFrom(player_initial_data);
    }

    initial_data_wrapper.set_type(Wrapper::WORLDDATA);
    initial_data_wrapper.set_payload(initial_data.SerializeAsString());

    return initial_data_wrapper;
}

void ChunkHeartbeat::do_heartbeat()
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
            auto position = ChunkHeartbeat::gather_player_transformation(player, true);
            chunkHeartbeat.add_players()->CopyFrom(position);
        }
    }

    Wrapper wrapper;
    wrapper.set_type(Wrapper::HEARTBEAT);
    wrapper.set_payload(chunkHeartbeat.SerializeAsString());
    std::string compressed_msg;
    BinaryCompressor compressor;
    compressor.compress_string(wrapper.SerializeAsString(), compressed_msg);

    send_data_to_players_in_chunk(compressed_msg);

    timer_.expires_after(boost::asio::chrono::milliseconds(tickrate_));
    timer_.async_wait([this](boost::system::error_code ec) {
        if (!ec) {
            do_heartbeat();
        }
        });
}

void ChunkHeartbeat::send_data_to_players_in_chunk(std::string& const message)
{
    for (auto& player : online_characters_) {
        server_.send_data_to_player(player->player_endpoint, message);
    }
}

PlayerPosition ChunkHeartbeat::gather_player_transformation(std::shared_ptr<ZoneCharacter> character, bool is_heartbeat)
{
    PlayerPosition transform;

    transform.set_player_id(character->get_player_id());
    transform.set_position_x(character->get_transform().x);
    transform.set_position_y(character->get_transform().y);
    transform.set_position_z(character->get_transform().z);

    if (is_heartbeat) {
        character->set_last_sent_position();
    }

    return transform;
}