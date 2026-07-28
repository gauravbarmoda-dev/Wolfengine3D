#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <cstdint>

class Rasterizer;
class AssetMgr;
class Palette;
class Camera;
class Map;

struct ColumnGeometry{
    int   drawStart;
    int   drawEnd;
    
    float distance;
    float wallX;

    unsigned char tileID;

    bool side;
};

struct RowGeometry{
    int32_t startFloorX;
    int32_t startFloorY;
    int32_t stepX;
    int32_t stepY;
};

class Raycaster{
private:
    int scrWidth;
    int scrHeight;

    ColumnGeometry* colBuffer;
    RowGeometry* rowBuffer;
    float* recipLUT;

public:
    Raycaster(int w, int h);
    ~Raycaster();

    void CalculateColumnGeometry(Camera* camera, Map* map);

    void CalculateRowGeometry(Camera* camera);

    ColumnGeometry* GetColBuffer() {return colBuffer;}
    RowGeometry* GetRowBuffer() {return rowBuffer;}
};

#endif
