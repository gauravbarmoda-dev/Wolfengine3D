#include "Rasterizer.h"
#include "Raycaster.h"
#include "AssetMgr.h"
#include "Palette.h"
#include "Camera.h"
#include "Map.h"
#include <cstdint>

Raycaster::Raycaster(int w, int h) : scrWidth(w), scrHeight(h) {
    zBuffer = new float[scrWidth];
}

Raycaster::~Raycaster(){
    delete[] zBuffer;
}

void Raycaster::Render(Camera* camera, Map* map, Palette* palette, Rasterizer* rasterizer, AssetMgr* assets){
    const unsigned char* mapData = map->GetRawData();
    int mapShift = map->GetMapShift();

    float invWidth = 2.0f / (float)scrWidth;

    #pragma omp parallel for
    for(int x = 0; x < scrWidth; x++){
        uint16_t color;
        int tile;

        float cameraX = invWidth * x  - 1.0f;                       // normalize camera plane

        Vector2 rayDir = camera->dir + (camera->plane * cameraX);   // parametric equation of line P(t) = C + (V * t)

        int mapX = (int)camera->pos.x;                              // floating position to 2d map position
        int mapY = (int)camera->pos.y;

        Vector2 deltaDist = {                                       // distance ray must travel in it's direction            
            (rayDir.x == 0.0f) ? 1e30f : std::abs(1.0f / rayDir.x), // to shift to next closest gridline
            (rayDir.y == 0.0f) ? 1e30f : std::abs(1.0f / rayDir.y)
        };

        Vector2 sideDist;                                           // distance ray must travel in it's direction
                                                                    // to shift to next closest gridline on the map 
        int stepX, stepY;

        // calculating if the indexing should be left or right for the sideDist
        if(rayDir.x < 0){
            stepX = -1;
            sideDist.x = (camera->pos.x - mapX) * deltaDist.x;
        }
        else{
            stepX = 1;
            sideDist.x = (mapX + 1.0f - camera->pos.x) * deltaDist.x;
        }

        if(rayDir.y < 0){
            stepY = -1;
            sideDist.y = (camera->pos.y - mapY) * deltaDist.y;
        }
        else{
            stepY = 1;
            sideDist.y = (mapY + 1.0f - camera->pos.y) * deltaDist.y;
        }

        int mapIndex = (mapY << mapShift) + mapX;
        int stepYIndex = stepY << mapShift;

        bool side = 0;           // records which axis the ray collided with last. 0 for x, 1 for y
        bool rayHit = false;

        while(!rayHit){
            if(sideDist.x < sideDist.y){
                sideDist.x += deltaDist.x;
                mapIndex +=stepX;
                side = 0;
            }
            else{
                sideDist.y += deltaDist.y;
                mapIndex += stepYIndex;
                side = 1;
            }
            
            tile = mapData[mapIndex];
            if(tile > 0){
                rayHit = true;
            }
        }

        // perpendicular distance to the wall, 
        // since we did not use pythagorus theorem to find the distance, there is minimal fish eye effect
        float wallDistance = (side == 0) ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);       
        if(wallDistance <= 0.0f) wallDistance = 0.3f;        

        zBuffer[x] = wallDistance;

        int vertHeight = (int)(scrHeight / wallDistance);

        int drawStart = (scrHeight >> 1) - (vertHeight >> 1);
        if(drawStart < 0) drawStart = 0;

        int drawEnd = (scrHeight >> 1) + (vertHeight >> 1);
        if(drawEnd >= scrHeight) drawEnd = scrHeight - 1;

        //calculate where the ray hit the wall exactly
        float wallX = (side == 0) ?     //0 - vertical wall, 1- horizontal wall
                      (camera->pos.y + rayDir.y * wallDistance) :
                      (camera->pos.x + rayDir.x * wallDistance)
        ;

        wallX -= (int)wallX;    // % of the way across the wall

        Texture* tex = assets->GetTexture(tile);

        if(tex != nullptr){
            int texX = (int)(wallX * (float)tex->width);    // % -> coordinate

            // flipping the texture if we looking at the back of the wall
            if(side == 0 && rayDir.x < 0) texX = tex->width - texX - 1;
            if(side == 1 && rayDir.y > 0) texX = tex->width - texX - 1;

            float exactVertHeight = (float)scrHeight / wallDistance;
            float step = (float)tex->height / exactVertHeight;

            //drawStart might change if standing too close (line 97)
            float exactDrawStart = ((float)scrHeight / 2.0f) - (exactVertHeight / 2.0f);
            float texPos = ((float)drawStart - exactDrawStart) * step;    // screenPixel -> texturePixels

            uint16_t* slice = tex->pixels + (texX * tex->height);   //transposing width
            
            rasterizer->DrawTexturedVLine(x, drawStart, drawEnd, texPos, step, slice);
        }
        else{
            color = palette->GetColor(tile);
            if(side == 1) color = (color >> 1) & 0x7BEF;
            rasterizer->DrawVLine(x, drawStart, drawEnd, color);
        }
    }

}
