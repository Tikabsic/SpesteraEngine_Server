#include "MovableObject.h"
#include "ZoneMap.h"

TDM::Vector3 MovableObject::get_transform() const
{
	return transform_.position;
}

void MovableObject::move_game_object()
{
	auto self = shared_from_this();
	map_.update_object_position_in_chunks(self);
}

