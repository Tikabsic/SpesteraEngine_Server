#include "ZoneCharacter.h"
#include "TDM.h"

ZoneCharacter::ZoneCharacter(float X, float Y, float Z, float movement_speed, uint16_t RY, uint16_t pid)
{
    transform_.position.x = X;
    transform_.position.y = Y;
    transform_.position.z = Z;
    transform_.rotation_Y = RY;
    player_id_ = pid;
    player_fov_ = TDM::FieldOfView(transform_.position);
    player_movement_speed = movement_speed;
}

ZoneCharacter::~ZoneCharacter()
{
}

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

    movementVector.x *= player_movement_speed;
    movementVector.z *= player_movement_speed;

    if (movementVector.y > 0) {

        movementVector.y *= player_movement_speed;
    }
    else {

        movementVector.y *= TDM::gravity_force_;
    }

    transform_.position += movementVector;
    player_fov_.RepositionFov(movementVector);

    std::cout << "Player position: (" << transform_.position.x << ", "
        << transform_.position.y << ", "
        << transform_.position.z << ")" << std::endl;

}

float ZoneCharacter::get_character_movement_speed()
{
    return player_movement_speed;
}

bool ZoneCharacter::is_character_moving()
{
    if (transform_.position.x != last_sent_position_.x
        || transform_.position.y != last_sent_position_.y
        || transform_.position.z != last_sent_position_.z) {
        return true;
    }
    return false;
}

void ZoneCharacter::set_last_sent_position()
{
    last_sent_position_ = transform_.position;
}
