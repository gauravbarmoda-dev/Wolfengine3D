#include "Font.h"
#include "Rasterizer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <iostream>

Rasterizer::Rasterizer() : width(0), height(0), texture(nullptr), pixels(nullptr) {}

Rasterizer::~Rasterizer(){
    CleanUp();
}

bool Rasterizer::Initialize(SDL_Renderer* renderer, int scrWidth, int scrHeight){
    width  = scrWidth;
    height = scrHeight;

    pixels = new uint16_t[width * height];

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );

    if(!texture){
        std::cerr << "Failed to create Texture: " << SDL_GetError() << "\n";
        return false;
    }

    return true;
}

void Rasterizer::CleanUp(){
    if(pixels != nullptr){
        delete[] pixels;
        pixels = nullptr;
    }
    if(texture != nullptr){
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    return;
}

void Rasterizer::Clear(uint16_t color){
    int size = width * height;
    std::fill_n(pixels, size, color);
}

void Rasterizer::ClearHorizon(uint16_t ceil, uint16_t floor){
    int totalPixels = width * height;
    int halfPixels  = totalPixels >> 1;

    std::fill_n(pixels, halfPixels, ceil);
    std::fill_n(pixels + halfPixels, halfPixels, floor);
}

void Rasterizer::DrawPixel(int x, int y, uint16_t color){
    if(x >= 0 && x < width && y >= 0 && y < height){
        pixels[y * width + x] = color;
    }
}

void Rasterizer::DrawChar(int x, int y, char c, uint16_t color){
    if(c < '0' || c > '9') return;
    uint64_t charData = font[c - '0'];

    for(int i = 0; i < 64; i++){
        if(charData & (1ULL << (63 - i))){
            int px = x + (i & 7);
            int py = y + (i >> 3);

            int pixIndex = py * width + px;
            if(pixIndex >= 0 && pixIndex < width * height){
                pixels[pixIndex] = color;
            }
        }
    }
}

void Rasterizer::DrawStr(int x, int y, const std::string& s, uint16_t color){
    int curX = x;
    for(char c : s){
        DrawChar(curX, y, c, color);
        curX += 8;
    }
}

void Rasterizer::DrawFPS(int fps, int x, int y, uint16_t color){
    int curX = x;
    
    if(fps >= 100){
        DrawChar(curX, y, '0' + ((fps / 100) % 10), color);
        curX += 8;
    }

    if(fps >= 10){
        DrawChar(curX, y, '0' + ((fps / 10) % 10), color);
        curX += 8;
    }

    DrawChar(curX, y, '0' + (fps % 10), color);
}

void Rasterizer::DrawRectangle(int x, int y, int w, int h, bool isFilled, uint16_t color){
    if(isFilled){
        for(int i = y; i < y + h; i++){
            DrawHLine(i, x, x + w, color);
        }
    }
    else{
        DrawHLine(y, x, x + w, color);
        DrawVLine(x, y, y + h, color);
        DrawVLine(x + w - 1, y , y + h, color);
        DrawHLine(y + h - 1, x, x + w, color);
    }
    return;
}

void Rasterizer::Present(SDL_Renderer* renderer){
    SDL_UpdateTexture(texture, nullptr, pixels, width * sizeof(uint16_t));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
