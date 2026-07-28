#ifndef RASTERIZER_H
#define RASTERIZER_H

#include "AssetMgr.h"
#include "Camera.h"
#include "Raycaster.h"
#include <string>
#include <cstdint>
#include <algorithm>

struct ColumnGeometry;
class AssetMgr;
class Palette;

class Rasterizer{
private:
    int width;
    int height;

    uint16_t* pixels;

public:
    Rasterizer();
    ~Rasterizer();

    bool Initialize(int scrWidth, int scrHeight);

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

    void DrawTexturedVLine(int x, int startY, int endY, float texPos, float texStep, uint16_t* slice);
    
    void DrawWalls(ColumnGeometry* colBuffer, AssetMgr* assets, Palette* palette);

    void DrawHorizon(ColumnGeometry* colBuffer, uint16_t ceil, uint16_t floor);

    void DrawTexturedHorizon(ColumnGeometry* colBuffer, RowGeometry* rowBuffer, Texture* floor, Texture* ceil);

    int GetWidth() const {return width;}
    int GetHeight() const {return height;}
    const uint16_t* GetPixels() const {return pixels;}
};

#endif
