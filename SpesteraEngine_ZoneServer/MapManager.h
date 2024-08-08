#pragma once

#include "HeartbeatsManager.h"

class MapManager
{
public:
	MapManager();
	~MapManager();

private:
	//Modules
	HeartbeatsManager heartbeats_manager_;
};

