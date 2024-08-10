#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

#include "NetworkProtocol.pb.h"
#include "HashSpecialization.h"
#include "TDM.h"

class ZoneMap;

class MovableObject : public std::enable_shared_from_this<MovableObject>
{
public:
    struct Transform
    {
        TDM::Vector3 position;
    };

    MovableObject(ZoneMap& map, float x, float y, float z, float movementspeed, int objid)
        : map_(map), transform_{ TDM::Vector3{x, y, z} }, movement_speed_(movementspeed), object_id_(objid) {}

    ~MovableObject() {}

    TDM::Vector3 get_transform() const;
    void move_game_object();

    std::unordered_set<CellKey> fov_;

protected:
    ZoneMap& map_;
    int object_id_;
    Transform transform_;
    float movement_speed_;
};