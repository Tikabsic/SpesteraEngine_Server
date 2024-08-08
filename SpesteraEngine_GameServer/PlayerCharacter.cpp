#include "PlayerCharacter.h"
#include "TDM.h"

PlayerCharacter::PlayerCharacter(float X, float Y, float Z, uint16_t pid)
{
	transform_.position.x = X;
	transform_.position.y = Y;
	transform_.position.z = Z;
	player_id_ = pid;
}

PlayerCharacter::~PlayerCharacter()
{

}

