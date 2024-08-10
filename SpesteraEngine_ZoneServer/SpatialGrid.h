#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cmath>
#include <memory>
#include <iostream>
#include "Chunk.h"

// Forward declaration
class MovableObject;

// Use pair as a key for unordered_map
using CellKey = std::pair<int, int>;

class SpatialGrid {
public:

    SpatialGrid(int cellsize, const int fov, std::unordered_map<CellKey, std::unique_ptr<Chunk>>* chunks)
        : cell_size_(cellsize), fov_(fov), grid_(chunks) {
        std::cout << "SpatialGrid initialized..." << std::endl;
    };
    ~SpatialGrid();

    const int cell_size_;
    const int fov_;

    std::unordered_map<CellKey,std::unique_ptr<Chunk>>* grid_;

    // Adds an object to the appropriate cell
    void add_object(std::shared_ptr<MovableObject> go);

    // Updates the list of chunks where the player is located
    void update_object_chunks(std::shared_ptr<MovableObject>& go);

    // Helper function to calculate the cell key
    CellKey getChunkKey(double x, double y) const;
};