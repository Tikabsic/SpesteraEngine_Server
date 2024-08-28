#pragma once

#include <string>

#include "GameCharacterRepository.h"
#include "DatabaseProtocol.pb.h"

class GameCharacterService
{
public:

	GameCharacterService(std::string uri);
	~GameCharacterService();

	DatabaseResponseWrapper get_character_list(DatabaseRequestWrapper& wrapper);

private:
	GameCharacterRepository* repository_;
};

