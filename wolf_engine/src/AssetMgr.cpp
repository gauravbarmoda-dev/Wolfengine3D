#include "AssetMgr.h"
#include <fstream>
#include <iostream>
#include "Map.h"

AssetMgr::AssetMgr() {}

AssetMgr::~AssetMgr() {
    for (auto& pair : loadedMaps) {
        delete pair.second;
    }
    loadedMaps.clear();
}

Map* AssetMgr::LoadMap(const char* filePath, int requestedSize){                       
    std::string pathKey(filePath);
    if (loadedMaps.find(pathKey) != loadedMaps.end()) {
        std::cout << "SUCCESS: Loaded " << filePath << " from cache!" << std::endl;
        return loadedMaps[pathKey];
    }

    if (requestedSize <= 0 || (requestedSize & (requestedSize - 1)) != 0) {     
        std::cout << "ERROR: Map size " << requestedSize << " is not a Power of 2! Defaulting to 64." << std::endl;                                               
        requestedSize = 64;                                                     
    }                                                                           
  
    Map* newMap = new Map(requestedSize);

    std::ifstream in(filePath);
    if (in.is_open()){
        for (int y = 0; y < requestedSize; y++){
            for (int x = 0; x < requestedSize; x++){
                int val;      
                if (!(in >> val)){
                    std::cout << "WARNING: Map file corrupted" << std::endl;    
                    goto end_load;
                }
                newMap->SetTile(x, y, (unsigned char)val);
                
                if (in.good() && in.peek() == ',') in.ignore();
            }
        }
        end_load:
        std::cout << "SUCCESS: Loaded " << filePath << " into a " << requestedSize << "x" << requestedSize << std::endl;
    } 
    else {
        std::cout << "Could not find " << filePath << ". Returning empty map." << std::endl;
    }

    loadedMaps[pathKey] = newMap;
    return newMap;
}
