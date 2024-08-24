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

	void add_session_to_chunk(const std::shared_ptr<Session>& session);
	void remove_session_from_chunk(const std::shared_ptr<Session>& session);

	void add_object_to_chunk(const std::shared_ptr<MovableObject>& go);
	void remove_object_from_chunk(const std::shared_ptr<MovableObject>& go, bool isplayer = false);
	bool check_for_object(std::shared_ptr<MovableObject> go);

	std::unordered_set<std::shared_ptr<MovableObject>>& get_chunk_objects();

	void set_chunk_heartbeat(ChunkHeartbeat* chunkheartbeat);
	std::vector<std::shared_ptr<Session>>& get_sessions();

	CellKey chunk_position_;
private:
	ChunkHeartbeat* chunk_heartbeat_ = nullptr;
	std::unordered_set<std::shared_ptr<MovableObject>> chunk_objects_;
	std::vector<std::shared_ptr<Session>> sessions_;
	int chunk_size_ = 0;
};

