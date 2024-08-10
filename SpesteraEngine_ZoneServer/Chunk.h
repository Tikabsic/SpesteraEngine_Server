#pragma once
#include <vector>
#include <unordered_set>

#include "MovableObject.h"
#include "ChunkHeartbeat.h"
using CellKey = std::pair<int, int>;

class Chunk
{
public:
	Chunk(int size, int x, int y);
	~Chunk();

	void add_object_to_chunk(std::shared_ptr<MovableObject> go);
	void remove_object_from_chunk(std::shared_ptr<MovableObject> go);

	bool check_for_object(std::shared_ptr<MovableObject> go);
	void set_chunk_heartbeat(ChunkHeartbeat* chunkheartbeat);

	CellKey chunk_position_;
private:
	ChunkHeartbeat* chunk_heartbeat_ = nullptr;
	std::unordered_set<std::shared_ptr<MovableObject>> chunk_objects_;
	int chunk_size_ = 0;
};

