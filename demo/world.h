#ifndef WORLD_H
#define WORLD_H

#include "../engine/shlong.h"
#include <vector>
#include <iostream>

struct Level {
    Map* map;
    std::vector<Texture*> floorTex;
    std::vector<Texture*> ceilTex;
};

class World{
private:
    std::vector<Level> levels;
    int currentLevel;

public:
    World() : currentLevel(0) {}
    ~World() = default;

    bool Initialize(AssetMgr* assets);

    void LoadLevel(int levelIndex) {
        if(levelIndex >= 0 && (size_t)levelIndex < levels.size()){
            currentLevel = levelIndex;
            std::cout << "Switched to Level: " << levelIndex << "\n";
        } else {
            std::cerr << "Error, Level Index: " << levelIndex << " out of bounds" << "\n";
        }
    }

    Map* GetMap(){
        if(levels.empty()) return nullptr;
        return levels[currentLevel].map;
    }

    std::vector<Texture*> Getfloor(){
        return levels[currentLevel].floorTex;
    }

    std::vector<Texture*> GetCeil(){
        return levels[currentLevel].ceilTex;
    }
};

#endif
