#include "ChunkHeartbeat.h"
#include "ZSProtocol.pb.h"
#include "BinaryCompressor.h"

ChunkHeartbeat::ChunkHeartbeat(boost::asio::io_context& io_context, int tickrate, std::vector<std::shared_ptr<Session>>& sessions, CellKey key)
    : timer_(io_context, boost::asio::chrono::milliseconds(tickrate)),
    tickrate_(tickrate), prev_timestamp(0), sessions_(sessions), chunk_key_(key)
{
    do_heartbeat();
}

ZSWrapper ChunkHeartbeat::gather_initial_world_data(uint16_t playerId) {
    ZSWrapper initial_data_wrapper;

    if (sessions_.empty()) {
        std::cout << "No online characters." << std::endl;
        return initial_data_wrapper;
    }

    WorldData initial_data;
    for (auto& player_data : sessions_) {

        if (playerId == player_data->get_player_id()) {
            continue;
        }
        PlayerInitialData player_initial_data;

        auto zone_character = player_data->get_zone_character();

        player_initial_data.set_player_id(player_data->get_player_id());
        player_initial_data.set_position_x(zone_character->get_transform().x);
        player_initial_data.set_position_y(zone_character->get_transform().y);
        player_initial_data.set_position_z(zone_character->get_transform().z);
        player_initial_data.set_player_movementspeed(zone_character->get_character_movement_speed());

        initial_data.add_players()->CopyFrom(player_initial_data);
    }

    initial_data_wrapper.set_type(ZSWrapper::WORLDDATA);
    initial_data_wrapper.set_payload(initial_data.SerializeAsString());

    return initial_data_wrapper;
}

void ChunkHeartbeat::do_heartbeat()
{
        if (sessions_.size() < 1) {
            timer_.expires_after(boost::asio::chrono::milliseconds(tickrate_));
            timer_.async_wait([this](boost::system::error_code ec) {
                if (!ec) {
                    do_heartbeat();
                }
                });
            return;
        }

    Heartbeat chunkHeartbeat;

    for (auto& player : sessions_) {
        if (player->get_zone_character()->is_character_moving()) {
            auto position = ChunkHeartbeat::gather_player_transformation(player->get_zone_character(), true);
            chunkHeartbeat.add_players()->CopyFrom(position);
        }
    }

    ZSWrapper wrapper;
    wrapper.set_type(ZSWrapper::HEARTBEAT);
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
    //std::cout << "message from :" << chunk_key_.first << " ," << chunk_key_.second << std::endl;
    for (auto& player : sessions_) {
        player->direct_push_to_buffer(message);
    }
}

PlayerPosition ChunkHeartbeat::gather_player_transformation(std::shared_ptr<ZoneCharacter>& character, bool is_heartbeat)
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