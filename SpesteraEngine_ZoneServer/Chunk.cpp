#include "Chunk.h"
#include <iostream>

Chunk::Chunk(int size, int x, int y) :
	chunk_position_({ x,y }), chunk_size_(size)
{
	std::cout << "Initiated chunk: ( " <<chunk_position_.first << " , " <<  chunk_position_.second << " )" << std::endl;
}

Chunk::~Chunk()
{

}

void Chunk::add_object_to_chunk(std::shared_ptr<MovableObject> go) {

    chunk_objects_.insert(go);

    //auto zone_character = std::dynamic_pointer_cast<ZoneCharacter>(go);
    //if (zone_character) {
    //    if (chunk_heartbeat_) {
    //        chunk_heartbeat_->push_zone_character(zone_character);
    //    }
    //}
    std::cout << "added to chunk" << "( " << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
}

void Chunk::remove_object_from_chunk(std::shared_ptr<MovableObject> go) {

    chunk_objects_.erase(go);

    //auto zone_character = std::dynamic_pointer_cast<ZoneCharacter>(go);
    //if (zone_character) {
    //    if (chunk_heartbeat_) {
    //        chunk_heartbeat_->remove_zone_character(zone_character);
    //    }
    //}
    std::cout << "removed from chunk" << "( " << chunk_position_.first << " , " << chunk_position_.second << " )" << std::endl;
}

bool Chunk::check_for_object(std::shared_ptr<MovableObject> go) {
	auto it = chunk_objects_.find(go);
	return it != chunk_objects_.end();
}

void Chunk::set_chunk_heartbeat(ChunkHeartbeat* chunkheartbeat)
{
	chunk_heartbeat_ = chunkheartbeat;
}
