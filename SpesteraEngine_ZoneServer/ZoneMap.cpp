#include <cmath>
#include <utility>
#include <iostream>

#include "ZoneMap.h"
#include "SpatialGrid.h"
#include "Chunk.h"
#include "MapReader.h"


ZoneMap::ZoneMap(int fov)
	: field_of_view(fov), chunks_()
{
	MapReader data("map.bin");
	MapData* map_data = data.get_map_data();

	int map_size = map_data->MapSize;
	int chunk_size = map_data->ChunkSize;

	// Compute the number of chunks in each direction
	int numChunks = std::ceil(static_cast<double>(map_size) / chunk_size);
	// Initialize chunks
	for (int x = 1; x < numChunks + 1; ++x) {
		for (int y = 1; y < numChunks + 1; ++y) {
			auto chunk = std::make_unique<Chunk>(chunk_size, x, y);
			CellKey key = std::make_pair(x, y);
			chunks_.emplace(key, std::move(chunk));
		}
	}

	map_grid_ = std::make_unique<SpatialGrid>(chunk_size, fov, chunks_);
}

ZoneMap::~ZoneMap() {
}

void ZoneMap::update_object_position_in_chunks(std::shared_ptr<MovableObject>& obj)
{
	map_grid_->update_chunk_position(obj);
}

void ZoneMap::spawn_movable_object_on_map(const std::shared_ptr<MovableObject>& obj)
{
	map_grid_->add_object(obj);
}

void ZoneMap::update_zone_character_fov(std::shared_ptr<ZoneCharacter>& obj)
{
	map_grid_->update_zone_character_fov(obj);
}

void ZoneMap::remove_session(const std::shared_ptr<Session>& session)
{
	std::cout << "Removing session :" << session->get_player_id() << std::endl;
	auto& zc = session->get_zone_character();

	for (const auto& chunk_id : zc->fov_) {
		auto it = chunks_.find(chunk_id);
		if (it != chunks_.end()) {
			it->second->remove_session_from_chunk(session);
			auto mo = std::dynamic_pointer_cast<MovableObject>(zc);
			it->second->remove_object_from_chunk(mo, true);
		}
	}
}

WorldData ZoneMap::grab_world_data(std::shared_ptr<ZoneCharacter>& obj) {
	WorldData world_data;

	for (const auto& chunk_id : obj->fov_) {
		auto it = chunks_.find(chunk_id);
		if (it != chunks_.end()) {

			const auto& objects = it->second->get_chunk_objects();
			for (const auto& object : objects) {

				if (object->get_object_id() == obj->get_object_id()) {
					continue;
				}

				PlayerInitialData* player_data = world_data.add_players();
				player_data->set_player_id(object->get_object_id());
				player_data->set_player_movementspeed(object->get_movementspeed());

				const auto& transform = object->get_transform();
				player_data->set_position_x(transform.x);
				player_data->set_position_y(transform.y);
				player_data->set_position_z(transform.z);
			}
		}
	}

	return world_data;
}

WorldData ZoneMap::grab_chunk_data(CellKey chunkposition, u_short id) const
{
	WorldData data;
	auto chunk = chunks_.find(chunkposition);
	if (chunk != chunks_.end()) {

		const auto& objects = chunk->second->get_chunk_objects();
		for (const auto& object : objects) {

			if (object->get_object_id() == id) {
				continue;
			}

			PlayerInitialData* player_data = data.add_players();
			player_data->set_player_id(object->get_object_id());
			player_data->set_player_movementspeed(object->get_movementspeed());

			const auto& transform = object->get_transform();
			player_data->set_position_x(transform.x);
			player_data->set_position_y(transform.y);
			player_data->set_position_z(transform.z);
		}
	}
	return WorldData();
}

std::vector<Chunk*> ZoneMap::get_zonemap_chunks()
{
	std::vector<Chunk*> chunks;
	for (auto& pair : chunks_) {
		chunks.push_back(pair.second.get());
	}
	return chunks;
}
