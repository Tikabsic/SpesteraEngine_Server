#pragma once

#include <memory>
#include <cstdint>
#include "NetworkProtocol.pb.h"
#include "TDM.h"

struct PlayerTransform
{
	TDM::Vector3 position;
	uint16_t rotation_Y;
};

class Player_Character : public std::enable_shared_from_this<Player_Character>
{
public:

	//Base
	Player_Character(float X, float Y, float Z, float movement_speed, uint16_t RY, uint16_t pid);
	~Player_Character();
	void move_player_character(PlayerPosition transform);

	//Helpers
	float get_player_movement_speed();
	bool is_character_moving();
	void set_last_sent_position();

	PlayerTransform player_transform_;
	uint16_t player_id_;
	TDM::Vector3 last_sent_position_;
private:
	TDM::FieldOfView player_fov_;
	float player_movement_speed;
};

