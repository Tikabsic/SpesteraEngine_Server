#ifndef SERVERHEARTBEAT_H
#define SERVERHEARTBEAT_H

#include <boost/asio.hpp>
#include <vector>

#include "UdpServer.h"
#include "NetworkProtocol.pb.h"
#include "ZoneCharacter.h"

class ChunkHeartbeat {
public:
    ChunkHeartbeat(boost::asio::io_context& io_context, UdpServer& udpserver, int tickrate);
    void push_zone_character(std::shared_ptr<ZoneCharacter> character);
    void remove_zone_character(std::shared_ptr<ZoneCharacter> character);
    Wrapper gather_initial_world_data(uint16_t playerId);

    void do_heartbeat();

    void send_data_to_players_in_chunk(std::string& const message);
private:
    PlayerPosition gather_player_transformation(std::shared_ptr<ZoneCharacter> character, bool is_heartbeat);

    UdpServer& server_;
    boost::asio::steady_timer timer_;
    int tickrate_;
    int64_t prev_timestamp;

    std::vector<std::shared_ptr<ZoneCharacter>> online_characters_;
    CellKey chunk_key_;
};

#endif // SERVERHEARTBEAT_H