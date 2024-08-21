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
        std::cout << "Added session: " << session->get_player_id() << " to chunk: (" << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
    }
    else {
        std::cout << "Session: " << session->get_player_id() << " is already in chunk: (" << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
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
        std::cout << "Removed session: " << session->get_player_id() << " from chunk: (" << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
    }
}

void Chunk::add_object_to_chunk(const std::shared_ptr<MovableObject>& go) {
    chunk_objects_.insert(go);
    go->current_chunk_ = chunk_position_;
    std::cout << "Added object with id: " << go->get_object_id() << " to chunk: (" << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
}

void Chunk::remove_object_from_chunk(const std::shared_ptr<MovableObject>& go, bool isplayer) {
    if (isplayer) {
        auto it = std::find(chunk_objects_.begin(), chunk_objects_.end(), go);
        if (it != chunk_objects_.end()) {
            chunk_objects_.erase(go);
            std::cout << "Removed zone character with id: " << go->get_object_id() << " from chunk: (" << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
        }
        else {
            std::cout << "zone character not found" << std::endl;
        }
    }
    else {
        chunk_objects_.erase(go);
        std::cout << "Removed object with id: " << go->get_object_id() << " from chunk: (" << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
    }
}

bool Chunk::check_for_object(std::shared_ptr<MovableObject> go) {
	auto it = chunk_objects_.find(go);
	return it != chunk_objects_.end();
}

void Chunk::set_chunk_heartbeat(ChunkHeartbeat* chunkheartbeat)
{
	chunk_heartbeat_ = chunkheartbeat;
}

std::vector<std::shared_ptr<Session>>& Chunk::get_sessions()
{
    return sessions_;
}
