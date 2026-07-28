#include "Raycaster.h"
#include "Camera.h"
#include "Map.h"
#include <cstdint>

Raycaster::Raycaster(int w, int h) : scrWidth(w), scrHeight(h) {
    colBuffer = new ColumnGeometry[scrWidth];
    
    int halfHeight = scrHeight >> 1;
    rowBuffer = new RowGeometry[halfHeight];

    recipLUT = new float[halfHeight];
    for(int y = 1; y < halfHeight; y++){
        recipLUT[y] = (0.5f * scrHeight) / (float)y;
    }
}

Raycaster::~Raycaster(){
    delete[] colBuffer;
    delete[] rowBuffer;
    delete[] recipLUT;
}

void Raycaster::CalculateColumnGeometry(Camera* camera, Map* map){
    
    const unsigned char* mapData = map->GetRawData();
    
    int mapShift = map->GetMapShift();

    float invWidth = 2.0f / (float)scrWidth;

    #pragma omp parallel for schedule(static)
    for(int x = 0; x < scrWidth; x++){
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
        unsigned char tile = 0;
        while(1){
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
            if(tile > 0) break;
        }

        // perpendicular distance to the wall, 
        // since we did not use pythagorus theorem to find the distance, there is minimal fish eye effect
        float wallDistance = (side == 0) ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);       
        if(wallDistance <= 0.0f) wallDistance = 0.3f;        

        //zBuffer[x] = wallDistance;

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

        // flipping the texture if we looking at the back of the wall
        if(side == 0 && rayDir.x < 0) wallX = 1.0f - wallX;
        if(side == 1 && rayDir.y > 0) wallX =  1.0f - wallX;
        
        colBuffer[x].distance  = wallDistance; 
        colBuffer[x].drawStart = drawStart; 
        colBuffer[x].drawEnd   = drawEnd; 
        colBuffer[x].tileID    = tile; 
        colBuffer[x].wallX     = wallX; 
        colBuffer[x].side      = side;
    }
}

void Raycaster::CalculateRowGeometry(Camera* camera){
    int horizon = scrHeight >> 1;

    float rayDirX0 = camera->dir.x - camera->plane.x;
    float rayDirY0 = camera->dir.y - camera->plane.y;
    float rayDirX1 = camera->dir.x + camera->plane.x;
    float rayDirY1 = camera->dir.y + camera->plane.y;

    #pragma omp parallel for schedule(static)
    for(int y = horizon + 1; y < scrHeight; y++){
        int p = y - horizon;          //cam position from the centre of screen

        float rowDistance = recipLUT[p];   //distance from cam to floor
    
        // how far we have to move in map for every 1 pixel on the screen
        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / scrWidth;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / scrWidth;

        // starting point
        float startFloorX = camera->pos.x + rowDistance * rayDirX0;
        float startFloorY = camera->pos.y + rowDistance * rayDirY0;

        int index = y - (horizon + 1);
        
        rowBuffer[index].startFloorX = (int32_t)(startFloorX * 65536.0f);
        rowBuffer[index].startFloorY = (int32_t)(startFloorY * 65536.0f);
        rowBuffer[index].stepX = (int32_t)(floorStepX * 65536.0f);
        rowBuffer[index].stepY = (int32_t)(floorStepY * 65536.0f);
    }
}
