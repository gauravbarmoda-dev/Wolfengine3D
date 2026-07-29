#ifndef ASSETMGR_H
#define ASSETMGR_H

#define MAX_TILE_SIZE   256

#include <string>
#include <unordered_map>
#include <cstdint>

struct Texture{
    int height, width;
    uint16_t* pixels;

    int shift;
    int mask;

    ~Texture() {if(pixels) delete[] pixels;}
};

class Map;

class AssetMgr{
private:
    std::unordered_map<std::string, Map*> loadedMaps;
    Texture* textures[MAX_TILE_SIZE];
public:
    AssetMgr();
    ~AssetMgr();

    Map* LoadMap(const char* filepath, int requestedSize);

    Texture* LoadTexture(unsigned char tileID, const char* filepath);
    
    Texture* GetTexture(unsigned char tileID) {return textures[tileID];}
};

#endif
