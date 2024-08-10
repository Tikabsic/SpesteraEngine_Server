#pragma once

#include <memory>
#include <cstdint>
#include "NetworkProtocol.pb.h"
#include "MovableObject.h"

struct Transform
{
    TDM::Vector3 position;
};

class ZoneCharacter : public MovableObject
{
public:
    // Constructor
    ZoneCharacter(ZoneMap& map, float x, float y, float z, float movementspeed, int pid)
        : MovableObject(map, x, y, z, movementspeed, pid) {}

    ~ZoneCharacter() {}

    void move_zone_character(PlayerPosition transform);

    // Helpers
    float get_character_movement_speed() const;
    bool is_character_moving() const;
    int get_player_id() const;
    void set_last_sent_position();

    TDM::Vector3 last_sent_position_;

    boost::asio::ip::udp::endpoint player_endpoint;
private:

};