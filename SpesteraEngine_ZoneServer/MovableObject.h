#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

#include "ZSProtocol.pb.h"
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

    MovableObject(ZoneMap& map, float x, float y, float z, float movementspeed, int objid);

    virtual ~MovableObject() {}

    TDM::Vector3 get_transform() const;
    int get_object_id() const;
    void update_chunk_position();

    CellKey current_chunk_;
protected:
    ZoneMap& map_;
    int object_id_;
    Transform transform_;
    float movement_speed_;
};