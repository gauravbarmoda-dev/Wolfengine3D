#include "AssetMgr.h"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_surface.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include "Map.h"

AssetMgr::AssetMgr() {
    for(int i = 0; i < MAX_TILE_SIZE; i++){
        textures[i] = nullptr;
    }
}

AssetMgr::~AssetMgr() {
    for (auto& pair : loadedMaps) {
        delete pair.second;
    }
    loadedMaps.clear();

    for(int i = 0; i < MAX_TILE_SIZE; i++){
        if(textures[i] != nullptr){
            delete textures[i];
            textures[i] = nullptr;
        }
    }
}

Map* AssetMgr::LoadMap(const char* filePath, int requestedSize){                       
    std::string pathKey(filePath);
    if (loadedMaps.find(pathKey) != loadedMaps.end()) {
        std::cout << "SUCCESS: Loaded " << filePath << " from cache!" << std::endl;
        return loadedMaps[pathKey];
    }

    if (requestedSize <= 0 || (requestedSize & (requestedSize - 1)) != 0) {     
        std::cerr << "ERROR: Map size " << requestedSize << " is not a Power of 2! Defaulting to 64." << std::endl;                                               
        requestedSize = 64;                                                     
    }                                                                           
  
    Map* newMap = new Map(requestedSize);

    std::ifstream in(filePath);
    if (in.is_open()){
        for (int y = 0; y < requestedSize; y++){
            for (int x = 0; x < requestedSize; x++){
                int val;      
                if (!(in >> val)){
                    std::cerr << "WARNING: Map file corrupted" << std::endl;    
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
        std::cerr << "Could not find " << filePath << ". Returning empty map." << std::endl;
    }

    loadedMaps[pathKey] = newMap;
    return newMap;
}

Texture* AssetMgr::LoadTexture(unsigned char tileID, const char* filepath){
    if(textures[tileID] != nullptr) return textures[tileID];

    SDL_Surface* surface = SDL_LoadBMP(filepath);
    if(!surface){
        std::cerr << "Failed to load Texture " << filepath << " - " << SDL_GetError() << "\n";
        return nullptr;
    }

    SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB565, 0);
    SDL_FreeSurface(surface);
    if(!converted) return nullptr;

    Texture* tex = new Texture();
    tex->width  = converted->h;
    tex->height = converted->w;

    int numBytes = tex->width * tex->height * sizeof(uint16_t);
    tex->pixels = new uint16_t[tex->width * tex->height];
    std::memcpy(tex->pixels, converted->pixels, numBytes);

    SDL_FreeSurface(converted);
    textures[tileID] = tex;
    return tex;
}
