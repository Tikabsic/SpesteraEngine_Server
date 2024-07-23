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

    do_heartbeat();
}

void ServerHeartbeat::push_player_character(std::shared_ptr<Player_Character> character)
{
    online_characters_.push_back(character);
}

void ServerHeartbeat::do_heartbeat()
{
        if (server_.get_endpoint_map().empty()) {
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
            auto position = ServerHeartbeat::gather_player_transformation(player);
            chunkHeartbeat.add_players()->CopyFrom(position);
        }
    }

    if (chunkHeartbeat.players_size() < 1) {

        timer_.expires_after(boost::asio::chrono::milliseconds(tickrate_));
        timer_.async_wait([this](boost::system::error_code ec) {
            if (!ec) {
                do_heartbeat();
            }
            });
        return;

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
PlayerPosition ServerHeartbeat::gather_player_transformation(std::shared_ptr<Player_Character> character)
{
    PlayerPosition transform;
    transform.set_player_id(character->player_id_);
    transform.set_position_x(character->player_transform_.position.x);
    transform.set_position_y(character->player_transform_.position.y);
    transform.set_position_z(character->player_transform_.position.z);

    character->set_last_sent_position();

    return transform;
}