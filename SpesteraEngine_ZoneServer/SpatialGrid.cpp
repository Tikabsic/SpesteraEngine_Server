#include <iostream>

#include "SpatialGrid.h"
#include "MovableObject.h"
#include "HashSpecialization.h"

SpatialGrid::~SpatialGrid()
{
    std::cout << "SpatialGrid destroyed..." << std::endl;
}

// Adds an object to the appropriate cell
void SpatialGrid::add_object(std::shared_ptr<MovableObject> go) {
    CellKey cell = getChunkKey(go->get_transform().x, go->get_transform().z);
    auto it = grid_->find(cell);
    if (it != grid_->end()) {
        it->second->add_object_to_chunk(go);
    }
}

// Updates the list of chunks where the player is located
void SpatialGrid::update_object_chunks(std::shared_ptr<MovableObject>& go) {
    // Calculate new chunks within the FOV
    std::unordered_set<CellKey> newChunksInFOV;
    int minX = static_cast<int>(std::floor((go->get_transform().x - fov_) / cell_size_));
    int maxX = static_cast<int>(std::floor((go->get_transform().x + fov_) / cell_size_));
    int minY = static_cast<int>(std::floor((go->get_transform().z - fov_) / cell_size_));
    int maxY = static_cast<int>(std::floor((go->get_transform().z + fov_) / cell_size_));

    if (minX == maxX && minY == maxY) {
        CellKey singleChunk = { minX, minY };

        // Check if the player is already in the single chunk
        auto it = grid_->find(singleChunk);
        if (it != grid_->end() && !go->fov_.empty()) {
            const auto& chunkInFov = *go->fov_.begin();
            if (chunkInFov == singleChunk) {
                return;
            }
        }
    }

    // Calculate the new set of chunks in the FOV
    for (int cx = minX; cx <= maxX; ++cx) {
        for (int cy = minY; cy <= maxY; ++cy) {
            // Check if chunk exists in the grid
            auto it = grid_->find({ cx, cy });
            if (it != grid_->end()) {
                newChunksInFOV.insert({ cx, cy });
            }
        }
    }

    // Add the player to new chunks if not already present
    for (const auto& chunk : newChunksInFOV) {
        auto it = grid_->find(chunk);
        if (it != grid_->end()) {
            auto& chunkPtr = it->second;
            if (!chunkPtr->check_for_object(go)) {
                chunkPtr->add_object_to_chunk(go);
            }
        }

        // Remove the player from old chunks that are now out of FOV
        for (const auto& oldChunk : go->fov_) {
            if (newChunksInFOV.find(oldChunk) == newChunksInFOV.end()) {
                auto it = grid_->find(oldChunk);
                if (it != grid_->end()) {
                    auto& oldChunkPtr = it->second;
                    oldChunkPtr->remove_object_from_chunk(go);
                }
            }
        }

        // Update the list of chunks in FOV
        go->fov_ = newChunksInFOV;
    }
}

// Helper function to calculate the cell key
CellKey SpatialGrid::getChunkKey(double x, double y) const {
    int cellX = static_cast<int>(x / cell_size_);
    int cellY = static_cast<int>(y / cell_size_);
    return { cellX, cellY };
}