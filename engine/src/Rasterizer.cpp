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

const float MAX_VIEW_DISTANCE = 12.0f;
const float MAX_VIEW_DISTANCE_SQR = 144.0f;

// Fast pixel Darkening
inline uint16_t ShadePixel(uint16_t color, int fog){
    if(fog >= 256) return color;
    if(fog <= 0) return 0x0000;

    uint32_t r = (color >> 11) & 0x1F;
    uint32_t g = (color >> 5) & 0x3F;
    uint32_t b = color & 0x1F;

    r = ((r * fog) + 128) >> 8;
    g = ((g * fog) + 128) >> 8;
    b = ((b * fog) + 128) >> 8;

    return (r << 11) | (g << 5) | b;
}

inline int CalculateFog(float distance){
    const float fogMin = 8.0f;
    const float fogMax = 12.0f;
    
    if(distance <= fogMin) return 256;
    if(distance >= fogMax) return 0;

    return 256 - (int)(((distance - fogMin) / (fogMax - fogMin)) * 256.0f); 
}

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


void Rasterizer::DrawTexturedVLine(int x, int startY, int endY, float texPos, float texStep, uint16_t* slice, int fog){
    uint16_t* __restrict__ dst = pixels;
    const uint16_t* __restrict__ src = slice;

    int pixIndex = startY * width + x;
    int32_t fixedPos  = (int32_t)(texPos * 65536.0f);
    int32_t fixedStep = (int32_t)(texStep * 65536.0f);

    for(int y = startY; y < endY; y++){
        dst[pixIndex] = ShadePixel(src[fixedPos >> 16], fog);
        fixedPos += fixedStep;
        pixIndex += width;
    }
}

void Rasterizer::DrawWalls(ColumnGeometry* colBuffer, AssetMgr* assets, Palette* palette, Camera* cam){
    #pragma omp parallel for schedule(dynamic, 8)
    for(int x = 0; x < width; x++){

        ColumnGeometry& column = colBuffer[x];
        if(column.distance >= MAX_VIEW_DISTANCE){
            DrawVLine(x, column.drawStart, column.drawEnd, 0x0000);
            continue;
        }

        Texture* tex = assets->GetTexture(column.tileID);

        if(tex != nullptr){
            int texX = (float)(column.wallX * tex->width);
        
            float exactVertHeight = (float)height / column.distance;
            float step = column.distance * ((float)tex->height / (float)height);
    
            float camOffset = cam->pitch + (cam->z / column.distance);
            float exactDrawStart = ((float)height * 0.5f) - (exactVertHeight * 0.5f) + camOffset;

            float texPos = ((float)column.drawStart - exactDrawStart) * step;

            int fog = CalculateFog(column.distance);

            uint16_t* slice = tex->pixels + (texX << tex->shift);
            DrawTexturedVLine(x, column.drawStart, column.drawEnd, texPos, step, slice, fog);
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

void Rasterizer::DrawTexturedHorizon(ColumnGeometry* colBuffer, RowGeometry* rowBuffer, Texture* floorTex, Texture* ceilTex, Camera* cam){
    if(!floorTex || !ceilTex) return;
    int horizon = (height >> 1) + cam->pitch;

    // telling compiller that it is safe to use SIMD instructions here
    uint16_t* __restrict__ dst = pixels;
    const uint16_t* __restrict__ floorPx = floorTex->pixels;
    const uint16_t* __restrict__ ceilPx  = ceilTex->pixels;

    int fShift = 16 - floorTex->shift;
    int cShift = 16 - ceilTex->shift;
    
    #pragma omp parallel for schedule(dynamic, 8)
    for(int y = 0; y < height; y++){
        if(y == horizon) continue;
       
        float dist = rowBuffer[y].distance;
        int rowOffset = y * width;

        if(y >= horizon){
            if(dist >= MAX_VIEW_DISTANCE){
                for(int x = 0; x < width; x++){
                    if(y >= colBuffer[x].drawEnd) dst[rowOffset + x] = 0x0000;
                }
            }
            else{
                int fog = CalculateFog(dist);
                

                int32_t floorX = rowBuffer[y].startFloorX;
                int32_t floorY = rowBuffer[y].startFloorY;
                int32_t stepX = rowBuffer[y].stepX;
                int32_t stepY = rowBuffer[y].stepY;

                for(int x = 0; x < width; x++){
                    if(y >= colBuffer[x].drawEnd){
                        uint16_t color = floorPx[(((floorY >> fShift) & floorTex->mask) << floorTex->shift) + ((floorX >> fShift) & floorTex->mask)];
                        dst[rowOffset + x] = ShadePixel(color, fog);
                    }
                    floorX += stepX;
                    floorY += stepY;
                }
            }
        }
        else{
            if(dist >= MAX_VIEW_DISTANCE){
                for(int x = 0; x < width; x++){
                    if(y < colBuffer[x].drawStart) dst[rowOffset + x] = 0x0000;
                }
            }
            else{
                int fog = CalculateFog(dist);


                int32_t floorX = rowBuffer[y].startFloorX;
                int32_t floorY = rowBuffer[y].startFloorY;
                int32_t stepX = rowBuffer[y].stepX;
                int32_t stepY = rowBuffer[y].stepY;

                for(int x = 0; x < width; x++){
                    if(y < colBuffer[x].drawStart){
                        uint16_t color = ceilPx[(((floorY >> cShift) & ceilTex->mask) << ceilTex->shift) + ((floorX >> cShift) & ceilTex->mask)];
                        dst[rowOffset + x] = ShadePixel(color, fog);
                    }
                    floorX += stepX;
                    floorY += stepY;
                }
            }
        }
    }    
}

void Rasterizer::DrawSprite(Sprite& sprite, Camera* cam, Raycaster* raycaster){
    SpriteFrame* frame = &sprite.sheet->frames[sprite.currentFrame];

    SpriteProjection proj;
    raycaster->ProjectSprite(sprite.x, sprite.y, cam, &proj, frame->width, frame->height);

    if(proj.distance <= 0.1f) return;
    if(proj.distance >= MAX_VIEW_DISTANCE) return;

    int drawStartX = std::max(0, proj.drawStartX);
    int drawEndX   = std::min(width, proj.drawEndX);

    int spriteWidth = proj.drawEndX - proj.drawStartX;
    if(spriteWidth <= 0) return;

    int32_t texX_step = (frame->width << 16) / spriteWidth;
    int32_t texX_FP   = (drawStartX - proj.drawStartX) * texX_step;
        
    int fog = CalculateFog(proj.distance);
    
    for(int x = drawStartX; x < drawEndX; x++){
        int texX = texX_FP >> 16;
        texX_FP += texX_step;

        if(proj.distance >= raycaster->GetColBuffer()[x].distance) continue;       

        SpriteColumn* col = &frame->columns[texX];
    
        if(col->numRuns == 0) continue;

        DrawVertSprite(x, proj.drawStartY, proj.drawEndY, col, frame->height, fog);
    }
}

void Rasterizer::DrawVertSprite(int x, int startY, int endY, SpriteColumn* column, int frameHeight, int fog){
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
            dst[pixIndex] = ShadePixel(src[texPos >> 16], fog);
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
        if(distSqr >= MAX_VIEW_DISTANCE_SQR) continue;

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
