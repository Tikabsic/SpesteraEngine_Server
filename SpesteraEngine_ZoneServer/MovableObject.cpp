#include "MovableObject.h"
#include "ZoneMap.h"

MovableObject::MovableObject(ZoneMap& map, float x, float y, float z, float movementspeed, int objid) :
	map_(map), movement_speed_(movementspeed), object_id_(objid)
{
	transform_.position.x = x;
	transform_.position.y = y;
	transform_.position.z = z;
}

TDM::Vector3 MovableObject::get_transform() const
{
	return transform_.position;
}

int MovableObject::get_object_id() const
{
	return object_id_;
}

void MovableObject::update_chunk_position()
{
	auto self = shared_from_this();
	map_.update_object_position_in_chunks(self);
}

