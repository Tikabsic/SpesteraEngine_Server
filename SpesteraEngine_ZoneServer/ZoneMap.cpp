#include <cmath>
#include <utility>
#include <iostream>

#include "ZoneMap.h"
#include "SpatialGrid.h"
#include "Chunk.h"
#include "MapReader.h"


ZoneMap::ZoneMap(int fov)
	: chunks_()
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

	map_grid_ = std::make_unique<SpatialGrid>(chunk_size, fov, &chunks_);
}

ZoneMap::~ZoneMap() {
}

void ZoneMap::update_object_position_in_chunks(std::shared_ptr<MovableObject>& obj)
{
	map_grid_->update_object_chunks(obj);
}

void ZoneMap::spawn_movable_object_on_map(std::shared_ptr<MovableObject>& obj)
{
	map_grid_->add_object(obj);
}

std::vector<Chunk*> ZoneMap::get_zonemap_chunks()
{
	std::vector<Chunk*> chunks;
	for (auto& pair : chunks_) {
		chunks.push_back(pair.second.get());
	}
	return chunks;
}
