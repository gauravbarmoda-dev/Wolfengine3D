#ifndef ASSETMGR_H
#define ASSETMGR_H

#include <string>
#include <unordered_map>

class Map;

class AssetMgr{
private:
    std::unordered_map<std::string, Map*> loadedMaps;

public:
    AssetMgr();
    ~AssetMgr();

    Map* LoadMap(const char* filepath, int requestedSize);
};

#endif
