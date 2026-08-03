#include "Rasterizer.h"
#include "Raycaster.h"
#include "AssetMgr.h"
#include "Palette.h"
#include "Sprite.h"
#include "Camera.h"
#include "Font.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <cstdint>
#include <cmath>

const int MAX_VISIBLE_SPRITES = 4096;
static SpriteDrawInfo drawList[MAX_VISIBLE_SPRITES];
static SpriteDrawInfo tempSwapList[MAX_VISIBLE_SPRITES];

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
    SpriteFrame* frame = &sprite.sheet->frames[sprite.currentFrame];

    SpriteProjection proj;
    raycaster->ProjectSprite(sprite.x, sprite.y, cam, &proj, frame->width, frame->height);

    if(proj.distance <= 0.1f) return;

    int drawStartX = std::max(0, proj.drawStartX);
    int drawEndX   = std::min(width, proj.drawEndX);

    int spriteWidth = proj.drawEndX - proj.drawStartX;
    if(spriteWidth <= 0) return;

    int32_t texX_step = (frame->width << 16) / spriteWidth;
    int32_t texX_FP   = (drawStartX - proj.drawStartX) * texX_step;

    for(int x = drawStartX; x < drawEndX; x++){
        int texX = texX_FP >> 16;
        texX_FP += texX_step;

        if(proj.distance >= raycaster->GetColBuffer()[x].distance) continue;       

        SpriteColumn* col = &frame->columns[texX];
    
        if(col->numRuns == 0) continue;

        DrawVertSprite(x, proj.drawStartY, proj.drawEndY, col, frame->height);
    }
}

void Rasterizer::DrawVertSprite(int x, int startY, int endY, SpriteColumn* column, int frameHeight){
    int spriteHeight = endY - startY;
    if(spriteHeight <= 0) return;

    int32_t step = (frameHeight * 65536) / spriteHeight;        // for runData index
    
    int32_t invStep = (spriteHeight << 16) / frameHeight;       //for screenY and runHeight

    for(int i = 0; i < column->numRuns; i++){
        SpriteRun& run = column->runs[i];
    
        int screenY = startY + ((run.start * invStep) >> 16);
        int runHeight = ((run.size * invStep) >> 16);
        int screenEndY = screenY + runHeight;

        int drawStartY = std::max(0, screenY);
        int drawEndY   = std::min(height, screenEndY);

        int32_t texPos = 0;
        if(screenY < 0){
            texPos += step * (-screenY);
        }

        // pointer aliasing SMID
        uint16_t* __restrict__ dst = pixels;
        const uint16_t* __restrict__ src = run.runData;

        int pixIndex = drawStartY * width + x;
        for(int y = drawStartY; y < drawEndY; y++){
            dst[pixIndex] = src[texPos >> 16];
            pixIndex += width;
            texPos += step;
        }
    }
}

void Rasterizer::DrawSprites(Camera* cam, Raycaster* raycaster){
    float halfFov = std::atan(cam->fov);                //converts fov length into radians 
    float padding = 0.4f;                               //little offset so sprites don't disappear immediately
    float threshold = cosf(halfFov + padding);          //cosine curve
    float thresholdSqr = threshold * threshold;

    int spriteCount = 0;

    for(Sprite* sprite : renderQueue){
        if(spriteCount >= MAX_VISIBLE_SPRITES) break;

        Vector2 spritePos(sprite->x, sprite->y);
        Vector2 spriteVec = spritePos - cam->pos;

        float dotUnnormalized = spriteVec.Dot(cam->dir);    // is sprite in direction of our camera dir
        if(dotUnnormalized <= 0.0f) continue;

        float distSqr = spriteVec.Dot(spriteVec);           //just pythagorus
        if(((dotUnnormalized * dotUnnormalized) / distSqr) < thresholdSqr) continue;

        uint32_t distBits;
        std::memcpy(&distBits, &distSqr, sizeof(uint32_t));

        drawList[spriteCount].sprite = sprite;
        drawList[spriteCount].disSqrBits = distBits;
        spriteCount++;
    }

    // Radix sort
    SpriteDrawInfo* src = drawList;
    SpriteDrawInfo* dst = tempSwapList;

    for(int shift = 0; shift <= 24; shift += 8){
        int count[256] = {0};

        for(int i = 0; i < spriteCount; i++){
            count[(src[i].disSqrBits >> shift) & 0xFF]++;
        }

        for(int i = 254; i >= 0; i--){
            count[i] += count[i + 1];
        }

        // descending order
        for(int i = spriteCount - 1; i >= 0; i--){
            int buckets = (src[i].disSqrBits >> shift) & 0xFF;
            dst[--count[buckets]] = src[i];
        }

        SpriteDrawInfo* temp = src;
        src = dst;
        dst = temp;
    }

    for(int i = 0; i < spriteCount; i++){
        DrawSprite(*(src[i].sprite), cam, raycaster);
    }

    renderQueue.clear();
}
