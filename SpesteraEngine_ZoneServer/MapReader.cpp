#include "MapReader.h"

MapReader::MapReader(const std::string& filePath) : filePath_(filePath) {
    read_map_data();
}

bool MapReader::read_map_data() {
    std::ifstream inputFile(filePath_, std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file: " << filePath_ << std::endl;
        return false;
    }

    inputFile.read(reinterpret_cast<char*>(&map_data_.MapSize), sizeof(map_data_.MapSize));
    inputFile.read(reinterpret_cast<char*>(&map_data_.MapHeight), sizeof(map_data_.MapHeight));
    inputFile.read(reinterpret_cast<char*>(&map_data_.ChunkSize), sizeof(map_data_.ChunkSize));

    int objectCount;
    inputFile.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));

    map_data_.MapObjects.resize(objectCount);

    for (int i = 0; i < objectCount; ++i) {
        Net_MapObject& obj = map_data_.MapObjects[i];
        inputFile.read(reinterpret_cast<char*>(&obj.Id), sizeof(obj.Id));
        inputFile.read(reinterpret_cast<char*>(obj.Position), sizeof(obj.Position));
        inputFile.read(reinterpret_cast<char*>(&obj.ColliderSize), sizeof(obj.ColliderSize));
    }

    inputFile.close();
    return true;
}

void MapReader::display_map_data() const {
    std::cout << "Map Size: " << map_data_.MapSize << std::endl;
    std::cout << "Map Height: " << map_data_.MapHeight << std::endl;
    std::cout << "Number of Objects: " << map_data_.MapObjects.size() << std::endl;

    for (const auto& obj : map_data_.MapObjects) {
        std::cout << "Object ID: " << obj.Id << std::endl;
        std::cout << "Position: (" << obj.Position[0] << ", " << obj.Position[1] << ", " << obj.Position[2] << ")" << std::endl;
        std::cout << "Collider Size: " << obj.ColliderSize << std::endl;
    }
}

MapData* MapReader::get_map_data()
{
    return &map_data_;
}
