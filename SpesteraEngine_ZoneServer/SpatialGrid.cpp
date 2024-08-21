#include <iostream>

#include "SpatialGrid.h"
#include "MovableObject.h"
#include "HashSpecialization.h"

SpatialGrid::~SpatialGrid()
{
    std::cout << "SpatialGrid destroyed..." << std::endl;
}

// Adds an object to the appropriate cell
void SpatialGrid::add_object(const std::shared_ptr<MovableObject>& go) {
    CellKey cell = getChunkKey(go->get_transform().x, go->get_transform().z);
    auto it = grid_.find(cell);
    if (it != grid_.end()) {
        it->second->add_object_to_chunk(go);
        auto zc = std::dynamic_pointer_cast<ZoneCharacter>(go);
        if (zc) {
            it->second->add_session_to_chunk(zc->get_session());
        }
    }
}

void SpatialGrid::update_chunk_position(const std::shared_ptr<MovableObject>& go)
{
    CellKey new_cell = getChunkKey(go->get_transform().x, go->get_transform().z);
    if (new_cell == go->current_chunk_) {
        return;
    }
    else {
        auto oldc = grid_.find(go->current_chunk_);
        if (oldc != grid_.end()) {
            oldc->second->remove_object_from_chunk(go);
        }
        auto newc = grid_.find(new_cell);
        if (newc != grid_.end()) {
            newc->second->add_object_to_chunk(go);
        }
    }
}

void SpatialGrid::update_zone_character_fov(std::shared_ptr<ZoneCharacter>& go) {
    // Calculate new chunks within the FOV
    std::unordered_set<CellKey> newChunksInFOV;
    int minX = static_cast<int>(std::floor((go->get_transform().x - fov_) / cell_size_) + 1);
    int maxX = static_cast<int>(std::floor((go->get_transform().x + fov_) / cell_size_) + 1);
    int minY = static_cast<int>(std::floor((go->get_transform().z - fov_) / cell_size_) + 1);
    int maxY = static_cast<int>(std::floor((go->get_transform().z + fov_) / cell_size_) + 1);

    // Calculate the new set of chunks in the FOV
    for (int cx = minX; cx <= maxX; ++cx) {
        for (int cy = minY; cy <= maxY; ++cy) {
            // Check if chunk exists in the grid
            auto it = grid_.find({ cx, cy });
            if (it != grid_.end()) {
                newChunksInFOV.insert({ cx, cy });
            }
        }
    }

    // Determine which chunks are new and which are no longer in the FOV
    std::unordered_set<CellKey> chunksToAdd;
    std::unordered_set<CellKey> chunksToRemove;

    // Chunks that are in new FOV but not in the current FOV (need to be added)
    for (const auto& chunk : newChunksInFOV) {
        if (go->fov_.find(chunk) == go->fov_.end()) {
            chunksToAdd.insert(chunk);
        }
    }

    // Chunks that are in the current FOV but not in the new FOV (need to be removed)
    for (const auto& oldChunk : go->fov_) {
        if (newChunksInFOV.find(oldChunk) == newChunksInFOV.end()) {
            chunksToRemove.insert(oldChunk);
        }
    }

    // Add the object to new chunks
    for (const auto& chunk : chunksToAdd) {
        auto it = grid_.find(chunk);
        if (it != grid_.end()) {
            auto& chunkPtr = it->second;
            std::cout << "Adding session to chunk (" << chunk.first << ", " << chunk.second << ")" << std::endl;
            chunkPtr->add_session_to_chunk(go->get_session());
        }
    }

    // Remove the object from old chunks that are now out of FOV
    for (const auto& chunk : chunksToRemove) {
        auto it = grid_.find(chunk);
        if (it != grid_.end()) {
            auto& oldChunkPtr = it->second;
            std::cout << "Removing session from chunk (" << chunk.first << ", " << chunk.second << ")" << std::endl;
            oldChunkPtr->remove_session_from_chunk(go->get_session());
        }
    }

    // Update the list of chunks in FOV
    go->fov_ = newChunksInFOV;
}

// Helper function to calculate the cell key
CellKey SpatialGrid::getChunkKey(double x, double y) const {
    int cellX = static_cast<int>(x / cell_size_);
    int cellY = static_cast<int>(y / cell_size_);
    return {cellX + 1, cellY + 1};
}