#include "Chunk.h"
#include <iostream>
#include <algorithm>

Chunk::Chunk(int size, int x, int y) :
	chunk_position_({ x,y }), chunk_size_(size)
{
	std::cout << "Initiated chunk: ( " <<chunk_position_.first << " , " <<  chunk_position_.second << " )" << std::endl;
}

Chunk::~Chunk()
{

}

void Chunk::add_session_to_chunk(const std::shared_ptr<Session>& session) {
    auto it = std::find(sessions_.begin(), sessions_.end(), session);

    if (it == sessions_.end()) {
        sessions_.push_back(session);
        session->send_chunk_data(chunk_position_);
    }

}

void Chunk::remove_session_from_chunk(const std::shared_ptr<Session>& session)
{
    auto it = std::find_if(sessions_.begin(), sessions_.end(),
        [&session](const std::shared_ptr<Session>& s) {
            return s == session;
        });

    if (it != sessions_.end()) {
        sessions_.erase(it);
    }
}

void Chunk::add_object_to_chunk(const std::shared_ptr<MovableObject>& go) {
    chunk_objects_.insert(go);
    go->current_chunk_ = chunk_position_;
}

void Chunk::remove_object_from_chunk(const std::shared_ptr<MovableObject>& go, bool isplayer) {
    if (isplayer) {
        auto it = std::find(chunk_objects_.begin(), chunk_objects_.end(), go);
        if (it != chunk_objects_.end()) {
            chunk_objects_.erase(go);
        }
    }
    else {
        chunk_objects_.erase(go);
    }
}

bool Chunk::check_for_object(std::shared_ptr<MovableObject> go) {
	auto it = chunk_objects_.find(go);
	return it != chunk_objects_.end();
}

std::unordered_set<std::shared_ptr<MovableObject>>& Chunk::get_chunk_objects()
{
    return chunk_objects_;
}

void Chunk::set_chunk_heartbeat(ChunkHeartbeat* chunkheartbeat)
{
	chunk_heartbeat_ = chunkheartbeat;
}

std::vector<std::shared_ptr<Session>>& Chunk::get_sessions()
{
    return sessions_;
}
