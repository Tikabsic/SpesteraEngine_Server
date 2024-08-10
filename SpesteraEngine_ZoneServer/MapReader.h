#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

struct Net_MapObject {
    int Id;
    float Position[3];
    float ColliderSize;
};

struct MapData {
    int MapSize;
    int MapHeight;
    int ChunkSize;
    std::vector<Net_MapObject> MapObjects;
};

class MapReader {
public:
    MapReader(const std::string& filePath);
    bool read_map_data();
    void display_map_data() const;

    MapData* get_map_data();

private:
    std::string filePath_;
    MapData map_data_;
};