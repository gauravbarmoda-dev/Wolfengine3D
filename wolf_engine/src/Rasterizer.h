#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <cstdint>
#include <algorithm>

struct SDL_Renderer;
struct SDL_Texture;

class Rasterizer{
private:
    int width;
    int height;

    SDL_Texture* texture;
    uint16_t* pixels;

public:
    Rasterizer();
    ~Rasterizer();

    bool Initialize(SDL_Renderer* renderer, int scrWidth, int scrHeight);

    void CleanUp();

    void Clear(uint16_t color);
    void ClearHorizon(uint16_t ceil, uint16_t floor);

    void DrawPixel(int x, int y, uint16_t color);
    
    void DrawRectangle(int x, int y, int w, int h, bool isFilled, uint16_t color);

    void DrawTexturedVLine(int x, int startY, int endY, int texID, int texX);

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

    void Present(SDL_Renderer* renderer);

    int GetWidth()  {return width;}
    int GetHeight() {return height;}
};

#endif
