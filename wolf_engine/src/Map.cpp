#include "Map.h"
#include <cmath>

Map::Map(int size) : mapSize(size), mapMask(mapSize - 1){
    mapShift = __builtin_ctz(mapSize);
    mapData.resize(mapSize * mapSize, 1);
}

Map::~Map() {}

void Map::SetTile(int x, int y, char tileID){
    if((x | y) & ~mapMask) return;
    mapData[(y << mapShift) + x] = tileID;
}

