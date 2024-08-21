#ifndef SERVERHEARTBEAT_H
#define SERVERHEARTBEAT_H

#include <boost/asio.hpp>
#include <vector>

#include "TcpServer.h"
#include "ZSProtocol.pb.h"
#include "ZoneCharacter.h"

class ChunkHeartbeat {
public:
    ChunkHeartbeat(boost::asio::io_context& io_context, int tickrate, std::vector<std::shared_ptr<Session>>& sessions, CellKey key);

    ZSWrapper gather_initial_world_data(uint16_t playerId);

    void do_heartbeat();

    void send_data_to_players_in_chunk(std::string& const message);
private:
    PlayerPosition gather_player_transformation(std::shared_ptr<ZoneCharacter>& character, bool is_heartbeat);

    boost::asio::steady_timer timer_;
    int tickrate_;
    int64_t prev_timestamp;

    std::vector<std::shared_ptr<Session>>& sessions_;
    CellKey chunk_key_;
};

#endif // SERVERHEARTBEAT_H