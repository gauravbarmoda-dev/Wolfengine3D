#include "Map.h"
#include <cmath>
#include <iostream>
#include <fstream>

Map::Map(const std::string& filePath, int requestedSize){
    if (requestedSize <= 0 || (requestedSize & (requestedSize - 1)) != 0) {
        std::cout << "ERROR: Map size " << requestedSize << " is not a Power of 2! Defaulting to 64." << std::endl;
        requestedSize = 64;
    }

    mapSize = requestedSize;
    mapMask = mapSize - 1;
    mapShift = __builtin_ctz(mapSize);
    mapData.resize(mapSize * mapSize, 0); 

    std::ifstream in(filePath);                                                                                           
                                                                                                                                             
    if (in.is_open()){                                                                                                                  
        for (int y = 0; y < mapSize; y++){                                                                                            
            for (int x = 0; x < mapSize; x++){                                                                                         
                int val;      
                
                if (!(in >> val)){
                    std::cout << "WARNING: Map file corrupted" << std::endl;
                    goto end_load;
                }                                                                                                               
                SetTile(x, y, (unsigned char)val);                                                                        
                                                                                                                                             
                while (in.good() && (in.peek() == ' ' || in.peek() == '\n' || in.peek() == '\r')) in.ignore();
                if (in.good() && in.peek() == ',') in.ignore();
            }
        }
        end_load:
        std::cout << "SUCCESS: Loaded " << filePath << " into a " << mapSize << "x" << mapSize << std::endl;
    } 
    else {
        std::cout << "Could not find " << filePath << ". Generating default borders." << std::endl;
        for(int y = 0; y < mapSize; y++){
            for(int x = 0; x < mapSize; x++){
                if (x == 0 || x == mapSize - 1 || y == 0 || y == mapSize - 1) {
                    SetTile(x, y, 1);
                }
            }
        }
    }
}

Map::~Map() {}

void Map::SetTile(int x, int y, char tileID){
    if((x | y) & ~mapMask) return;
    mapData[(y << mapShift) + x] = tileID;
}

