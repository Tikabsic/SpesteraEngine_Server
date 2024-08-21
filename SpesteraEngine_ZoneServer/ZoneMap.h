#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "SpatialGrid.h"
#include "Chunk.h"


class ZoneMap : public std::enable_shared_from_this<ZoneMap>{
public:
	ZoneMap(int fov);
	~ZoneMap();

	void update_object_position_in_chunks(std::shared_ptr<MovableObject>& obj);
	void spawn_movable_object_on_map(const std::shared_ptr<MovableObject>& obj);

	void update_zone_character_fov(std::shared_ptr<ZoneCharacter>& obj);

	void remove_session(const std::shared_ptr<Session>& session);

	std::vector<Chunk*> get_zonemap_chunks();

private:
	int field_of_view;
	std::unique_ptr<SpatialGrid> map_grid_;
	std::unordered_map<CellKey, std::unique_ptr<Chunk>> chunks_;
};

