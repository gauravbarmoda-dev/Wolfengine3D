#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>

struct ColumnGeometry;
struct RowGeometry;
struct Texture;
struct SpriteColumn;
struct Sprite;
class Raycaster;
class AssetMgr;
class Palette;
class Camera;


class Rasterizer{
private:
    int width;
    int height;

    uint16_t* pixels;

    std::vector<Sprite*> renderQueue;

public:
    Rasterizer();
    ~Rasterizer();

    bool Initialize(int scrWidth, int scrHeight);

    void QueueSprite(Sprite* sprite) {renderQueue.push_back(sprite);}

    void CleanUp();

    void Clear(uint16_t color);

    void DrawPixel(int x, int y, uint16_t color);
    
    void DrawFPS(int fps, int x, int y, uint16_t color);

    void DrawChar(int x, int y, char c, uint16_t color);
    
    void DrawStr(int x, int y, const std::string& s, uint16_t color);
    
    void DrawRectangle(int x, int y, int w, int h, bool isFilled, uint16_t color);

    inline void DrawVLine(int x, int startY, int endY, uint16_t color){
        int pixIndex = startY * width + x;
        for(int y = startY; y < endY; y++){
            pixels[pixIndex] = color;
            pixIndex += width;
        } 
    }

    inline void DrawHLine(int y, int startX, int endX, uint16_t color){
        int startIndex = y * width + startX;
        int endIndex   = y * width + endX;

        std::fill(pixels + startIndex, pixels + endIndex, color);
    }

    void DrawTexturedVLine(int x, int startY, int endY, float texPos, float texStep, uint16_t* slice, int fog);
    
    void DrawWalls(ColumnGeometry* colBuffer, AssetMgr* assets, Palette* palette, Camera* cam);

    void DrawHorizon(ColumnGeometry* colBuffer, uint16_t ceil, uint16_t floor);

    void DrawTexturedHorizon(ColumnGeometry* colBuffer, RowGeometry* rowBuffer, Texture* floor, Texture* ceil, Camera* cam);

    void DrawVertSprite(int x, int startY, int endY, SpriteColumn* column, int frameHeight, int fog);

    void DrawSprite(Sprite& sprite, Camera* cam, Raycaster* raycaster);

    void DrawSprites(Camera* cam, Raycaster* raycaster);

    int GetWidth() const {return width;}
    int GetHeight() const {return height;}
    const uint16_t* GetPixels() const {return pixels;}
};

#endif
