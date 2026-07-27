#ifndef RAYCASTER_H
#define RAYCASTER_H

class Rasterizer;
class AssetMgr;
class Palette;
class Camera;
class Map;

class Raycaster{
private:
    int scrWidth;
    int scrHeight;

    float* zBuffer;

public:
    Raycaster(int w, int h);
    ~Raycaster();

    void Render(Camera* camera, Map* map, Palette* palette, Rasterizer* rasterizer, AssetMgr* assets);
};

#endif
