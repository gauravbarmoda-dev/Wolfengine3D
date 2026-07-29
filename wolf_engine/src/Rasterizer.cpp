#include "Rasterizer.h"
#include "Raycaster.h"
#include "AssetMgr.h"
#include "Palette.h"
#include "Sprite.h"
#include "Camera.h"
#include "Font.h"
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <cmath>

Rasterizer::Rasterizer() : width(0), height(0), pixels(nullptr) {}

Rasterizer::~Rasterizer(){
    CleanUp();
}

bool Rasterizer::Initialize(int scrWidth, int scrHeight){
    width  = scrWidth;
    height = scrHeight;

    pixels = new uint16_t[width * height];
    return true;
}

void Rasterizer::CleanUp(){
    if(pixels != nullptr){
        delete[] pixels;
        pixels = nullptr;
    }
    return;
}

void Rasterizer::Clear(uint16_t color){
    int size = width * height;
    std::fill_n(pixels, size, color);
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


void Rasterizer::DrawTexturedVLine(int x, int startY, int endY, float texPos, float texStep, uint16_t* slice){
    uint16_t* __restrict__ dst = pixels;
    const uint16_t* __restrict__ src = slice;

    int pixIndex = startY * width + x;
    int32_t fixedPos  = (int32_t)(texPos * 65536.0f);
    int32_t fixedStep = (int32_t)(texStep * 65536.0f);

    for(int y = startY; y < endY; y++){
        dst[pixIndex] = src[fixedPos >> 16];
        fixedPos += fixedStep;
        pixIndex += width;
    }
}

void Rasterizer::DrawWalls(ColumnGeometry* colBuffer, AssetMgr* assets, Palette* palette){
    #pragma omp parallel for schedule(dynamic, 8)
    for(int x = 0; x < width; x++){
        ColumnGeometry& column = colBuffer[x];

        Texture* tex = assets->GetTexture(column.tileID);

        if(tex != nullptr){
            int texX = (float)(column.wallX * tex->width);
        
            float exactVertHeight = (float)height / column.distance;
            float step = column.distance * ((float)tex->height / (float)height);
            float exactDrawStart = ((float)height * 0.5f) - (exactVertHeight * 0.5f);

            float texPos = ((float)column.drawStart - exactDrawStart) * step;
    
            uint16_t* slice = tex->pixels + (texX << tex->shift);

            DrawTexturedVLine(x, column.drawStart, column.drawEnd, texPos, step, slice);
        }
        else{
            uint16_t color = palette->GetColor(column.tileID);
            if(column.side == 1) color = (color >> 1) & 0x7BEF;
            DrawVLine(x, column.drawStart, column.drawEnd, color);
        }
    }
}

void Rasterizer::DrawHorizon(ColumnGeometry* colBuffer, uint16_t ceil, uint16_t floor){
    for(int x = 0; x < width; x++){
        ColumnGeometry& col = colBuffer[x];

        DrawVLine(x, 0, col.drawStart, ceil);
        DrawVLine(x, col.drawEnd, height, floor);
    } 
}

void Rasterizer::DrawTexturedHorizon(ColumnGeometry* colBuffer, RowGeometry* rowBuffer, Texture* floorTex, Texture* ceilTex){
    if(!floorTex || !ceilTex) return;

    int horizon = height >> 1;

    // telling compiller that it is safe to use SIMD instructions here
    uint16_t* __restrict__ dst = pixels;
    const uint16_t* __restrict__ floorPx = floorTex->pixels;
    const uint16_t* __restrict__ ceilPx  = ceilTex->pixels;

    int fcoordShift = 16 - floorTex->shift;
    int cccordShift = 16 - ceilTex->shift;
    
    #pragma omp parallel for schedule(dynamic, 8)
    for(int y = horizon + 1; y < height; y++){
        int index = y - (horizon + 1);

        int32_t floorX = rowBuffer[index].startFloorX;
        int32_t floorY = rowBuffer[index].startFloorY;
        int32_t stepX = rowBuffer[index].stepX;
        int32_t stepY = rowBuffer[index].stepY;

        int rowOffset = y * width;
        
        for(int x = 0; x < width; x++){
            if(y >= colBuffer[x].drawEnd){
                int tx = (floorX >> fcoordShift) & (floorTex->mask);
                int ty = (floorY >> fcoordShift) & (floorTex->mask);

                dst[rowOffset + x] = floorPx[(ty << floorTex->shift) + tx];
            }
            floorX += stepX;
            floorY += stepY;
        }
    }    
    
    #pragma omp parallel for schedule(dynamic, 8)
    for(int y = horizon + 1; y < height; y++){
        int index = y - (horizon + 1);

        int32_t floorX = rowBuffer[index].startFloorX;
        int32_t floorY = rowBuffer[index].startFloorY;
        int32_t stepX = rowBuffer[index].stepX;
        int32_t stepY = rowBuffer[index].stepY;

        int ceilY = height - y - 1;
        int rowOffset = ceilY * width;
        
        for(int x = 0; x < width; x++){
            if(ceilY < colBuffer[x].drawStart){
                int tx = (floorX >> cccordShift) & (ceilTex->mask);
                int ty = (floorY >> cccordShift) & (ceilTex->mask);

                dst[rowOffset + x] = ceilPx[(ty << ceilTex->shift) + tx];
            }
            floorX += stepX;
            floorY += stepY;
        }
    }
}

void Rasterizer::DrawSprite(Sprite& sprite, Camera* cam, Raycaster* raycaster){
    SpriteProjection proj;
    raycaster->ProjectSprite(sprite.x, sprite.y, cam, &proj);

    if(proj.distance <= 0.1f) return;

    int drawStartX = std::max(0, proj.drawStartX);
    int drawEndX   = std::min(width, proj.drawEndX);

    int spriteWidth = proj.drawEndX - proj.drawStartX;

    SpriteFrame* frame = &sprite.sheet->frames[sprite.currentFrame];

    for(int x = drawStartX; x < drawEndX; x++){
        int texX = (x - proj.drawStartX) * frame->width / spriteWidth;

        SpriteColumn* col = &frame->columns[texX];

        DrawVertSprite(x, proj.drawStartY, proj.drawEndY, col, proj.distance, raycaster->GetColBuffer(), frame->height);
    }
}

void Rasterizer::DrawVertSprite(int x, int startY, int endY, SpriteColumn* column, float spriteDistance, ColumnGeometry* colBuffer, int frameHeight){
    if(spriteDistance >= colBuffer[x].distance) return;
    if(column->numRuns == 0) return;

    int spriteHeight = endY - startY;
    int32_t step = (frameHeight * 65536) / spriteHeight;

    for(int i = 0; i < column->numRuns; i++){
        SpriteRun& run = column->runs[i];
    
        int screenY = startY + (run.start * spriteHeight) / frameHeight;
        int runHeight = (run.size * spriteHeight) / frameHeight;
        int screenEndY = screenY + runHeight;

        int drawStartY = std::max(0, screenY);
        int drawEndY   = std::min(height, screenEndY);

        int32_t texPos = 0;
        if(screenY < 0){
            texPos += step * (-screenY);
        }

        for(int y = drawStartY; y < drawEndY; y++){
            uint16_t color = run.runData[texPos >> 16];
            pixels[y * width + x] = color;
            texPos += step;
        }
    }
}
