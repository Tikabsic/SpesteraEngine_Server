#pragma once

#include <memory>
#include <cstdint>
#include "ZSProtocol.pb.h"
#include "MovableObject.h"

class Session;

struct Transform
{
    TDM::Vector3 position;
};

class ZoneCharacter : public MovableObject
{
public:

    ZoneCharacter(ZoneMap& map, float x, float y, float z, float movementspeed, int pid, std::weak_ptr<Session> session)
        : MovableObject(map, x, y, z, movementspeed, pid), session_(session) {}

    ~ZoneCharacter() { std::cout << "Zone_Character deleted : " << object_id_ << std::endl; }

    std::shared_ptr<Session> get_session() const {
        return session_.lock();
    }

    void move_zone_character(PlayerPosition transform);
    void update_fov();

    // Helpers

    float get_character_movement_speed() const;
    bool is_character_moving() const;
    int get_player_id() const;
    void set_last_sent_position();
    void delete_session_ptr();


    TDM::Vector3 last_sent_position_;
    std::unordered_set<CellKey> fov_;
private:
    std::weak_ptr<Session> session_;
};