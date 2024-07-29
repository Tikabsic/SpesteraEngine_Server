#include "Player_Character.h"
#include "TDM.h"

Player_Character::Player_Character(float X, float Y, float Z, float movement_speed, uint16_t RY, uint16_t pid)
{
	player_transform_.position.x = X;
	player_transform_.position.y = Y;
	player_transform_.position.z = Z;
	player_transform_.rotation_Y = RY;
	player_id_ = pid;
    player_fov_ = TDM::FieldOfView(player_transform_.position);
    player_movement_speed = movement_speed;
}

Player_Character::~Player_Character()
{
}

void Player_Character::move_player_character(PlayerPosition transform)
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

    if (movementVector.y < 0) {
        movementVector.y *= TDM::gravity_force;
    }
    else {
        movementVector.y *= player_movement_speed;
    }

    movementVector.z *= player_movement_speed;

    player_transform_.position += movementVector;
    player_fov_.RepositionFov(movementVector);

    std::cout << "Player position: (" << player_transform_.position.x << ", "
        << player_transform_.position.y << ", "
        << player_transform_.position.z << ")" << std::endl;

}

float Player_Character::get_player_movement_speed()
{
    return player_movement_speed;
}

bool Player_Character::is_character_moving()
{
    if (player_transform_.position.x != last_sent_position_.x
        || player_transform_.position.y != last_sent_position_.y
        || player_transform_.position.z != last_sent_position_.z) {
        return true;
    }
    return false;
}

void Player_Character::set_last_sent_position()
{
    last_sent_position_ = player_transform_.position;
}

