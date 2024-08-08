#ifndef SERVERHEARTBEAT_H
#define SERVERHEARTBEAT_H

#include <boost/asio.hpp>
#include <vector>

#include "UdpServer.h"
#include "NetworkProtocol.pb.h"
#include "PlayerCharacter.h"

class ServerHeartbeat {
public:
    ServerHeartbeat(boost::asio::io_context& io_context, UdpServer& udpserver, int tickrate);
    void push_player_character(std::shared_ptr<PlayerCharacter> character);
    void remove_player_character(std::shared_ptr<PlayerCharacter> character);
    Wrapper gather_initial_world_data(uint16_t playerId);
private:
    void do_heartbeat();
    PlayerPosition gather_player_transformation(std::shared_ptr<PlayerCharacter> character, bool is_heartbeat);

    UdpServer& server_;
    boost::asio::steady_timer timer_;
    int tickrate_;
    int64_t prev_timestamp;

    std::vector<std::shared_ptr<PlayerCharacter>> online_characters_;
    std::mutex characters_mutex_;
};

#endif // SERVERHEARTBEAT_H