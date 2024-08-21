#pragma once

#include <memory>
#include <cstdint>
#include "GSProtocol.pb.h"
#include "TDM.h"

struct Transform
{
	TDM::Vector3 position;
};

class PlayerCharacter : public std::enable_shared_from_this<PlayerCharacter>
{
public:

	//Base
	PlayerCharacter(float X, float Y, float Z, uint16_t pid);
	~PlayerCharacter();

	//Helpers

	Transform transform_;
	uint16_t player_id_;
private:

};

