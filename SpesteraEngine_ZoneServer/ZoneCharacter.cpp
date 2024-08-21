#include "ZoneCharacter.h"
#include "ZoneMap.h"
#include "TDM.h"


void ZoneCharacter::move_zone_character(PlayerPosition transform)
{
    TDM::Vector3 movementVector;
    if (transform.position_x() != 0)
    {
        movementVector.x = transform.position_x();
    }
    if (transform.position_y() != 0)
    {
        movementVector.y = transform.position_y();
    }
    if (transform.position_z() != 0)
    {
        movementVector.z = transform.position_z();
    }

    movementVector.x *= movement_speed_;
    movementVector.z *= movement_speed_;

    if (movementVector.y > 0) {

        movementVector.y *= movement_speed_;
    }
    else {

        movementVector.y *= TDM::gravity_force_;
    }

    transform_.position += movementVector;
    update_chunk_position();
    update_fov();
}

void ZoneCharacter::update_fov()
{
    auto self = std::dynamic_pointer_cast<ZoneCharacter>(shared_from_this());
    if (self) {
        map_.update_zone_character_fov(self);
    }
}

float ZoneCharacter::get_character_movement_speed() const
{
    return movement_speed_;
}

bool ZoneCharacter::is_character_moving() const
{
    if (transform_.position.x != last_sent_position_.x
        || transform_.position.y != last_sent_position_.y
        || transform_.position.z != last_sent_position_.z) {
        return true;
    }
    return false;
}

int ZoneCharacter::get_player_id() const
{
    return object_id_;
}

void ZoneCharacter::set_last_sent_position()
{
    last_sent_position_ = transform_.position;
}
