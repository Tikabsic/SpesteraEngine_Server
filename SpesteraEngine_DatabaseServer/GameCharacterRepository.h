#pragma once

#include "Repository.h"

class GameCharacterRepository : public Repository
{
public:

	GameCharacterRepository(std::string uri) : Repository(uri) {};
	~GameCharacterRepository() {};



private:
};

