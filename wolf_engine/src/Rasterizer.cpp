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

void Rasterizer::DrawTexturedVLine(int x, int startY, int endY, int texID, int texX){
    return ;    
}

void Rasterizer::Present(SDL_Renderer* renderer){
    SDL_UpdateTexture(texture, nullptr, pixels, width * sizeof(uint16_t));
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
