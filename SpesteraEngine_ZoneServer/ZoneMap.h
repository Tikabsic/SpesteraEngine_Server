#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "SpatialGrid.h"
#include "Chunk.h"


class ZoneMap
{
public:
	ZoneMap(int fov);
	~ZoneMap();

	void update_object_position_in_chunks(std::shared_ptr<MovableObject>& obj);
	void spawn_movable_object_on_map(std::shared_ptr<MovableObject>& obj);

	std::vector<Chunk*> get_zonemap_chunks();

private:
	std::unique_ptr<SpatialGrid> map_grid_;
	std::unordered_map<CellKey, std::unique_ptr<Chunk>> chunks_;
};

