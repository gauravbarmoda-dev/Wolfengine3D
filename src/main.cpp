#include "../wolf_engine/src/Rasterizer.h"
#include "../wolf_engine/src/Raycaster.h"
#include "../wolf_engine/src/AssetMgr.h"
#include "../wolf_engine/src/Palette.h"
#include "../wolf_engine/src/Camera.h"
#include "../wolf_engine/src/Engine.h"
#include "../wolf_engine/src/Map.h"
#include "Player.h"
#include "../wolf_engine/src/Sprite.h"
#include <cmath>

// Helper function to figure out which side of Snorlax the player is looking at (8-way)
int GetDirectionalRow(float spriteX, float spriteY, float spriteAngle, float camX, float camY) {
    float dx = camX - spriteX;
    float dy = camY - spriteY;
    
    float angleToPlayer = atan2(dy, dx); 
    float diff = angleToPlayer - spriteAngle;
    
    while (diff <= -M_PI) diff += 2.0f * M_PI;
    while (diff > M_PI) diff -= 2.0f * M_PI;

    // Divide a circle into 8 slices of 45 degrees (PI/4 radians)
    // Offset by PI/8 so that "Front" is centered exactly at 0.
    float sector = (diff + (M_PI / 8.0f)) / (M_PI / 4.0f);
    
    int dir = (int)floor(sector);
    if (dir < 0) dir += 8;

    // Standard order (e.g. Pokemon Mystery Dungeon sheets):
    // 0=Down, 1=DownRight, 2=Right, 3=UpRight, 4=Up, 5=UpLeft, 6=Left, 7=DownLeft
    // If Snorlax looks wrong, you can swap these numbers around!
    int sheetOrder[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    
    return sheetOrder[dir];
}

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

const uint16_t SKY_COLOR   = 0x2945; 
const uint16_t FLOOR_COLOR = 0x4184;

int main() {
    Raycaster  raycaster(SCREEN_WIDTH, SCREEN_HEIGHT);
    Rasterizer rasterizer;
    Palette    palette;
    Engine     engine;
    AssetMgr   assets;

    if (!engine.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Wolf Engine - 3D Dungeon")) {
        return -1;
    }

    rasterizer.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);
    engine.TargetFPS(100);

    Map* map = assets.LoadMap("assets/maps/level2.map", 32);

    assets.LoadTexture(1, "assets/sprites/brick_wall.bmp");
    assets.LoadTexture(2, "assets/sprites/pop_wall.bmp");
    assets.LoadTexture(3, "assets/sprites/wooden_door.bmp");
    assets.LoadTexture(4, "assets/sprites/iron_wall.bmp");
    assets.LoadTexture(5, "assets/sprites/iron_wall_disorderd.bmp");
    assets.LoadTexture(6, "assets/sprites/ancient_wall.bmp");
    assets.LoadTexture(7, "assets/sprites/cement_wall.bmp");
    assets.LoadTexture(8, "assets/sprites/floor_grass.bmp");
    assets.LoadTexture(9, "assets/sprites/cement_ceil.bmp");

    // Load Snorlax (4 columns, 8 rows). Once you fix the image, it should be 256x512 (64x64 per sprite!)
    // 0xF81F is the RGB565 hex code for pure Magenta.
    SpriteSheet* snorlaxSheet = assets.LoadSpriteSheet("assets/sprites/snorlax_walk.bmp", 64, 64, 0xF81F);

    // Load Spiritomb (Single static 64x64 sprite!)
    SpriteSheet* spiritombSheet = assets.LoadSpriteSheet("assets/sprites/spiritomb.bmp", 64, 64, 0xF81F);

    Camera camera;
    Player player(Vector2(2.5f, 2.5f), 0.0f, 0.2f);

    // Place Snorlax directly East of the player's starting position
    // Since player is at (2.5, 2.5), we put him at (4.5, 2.5) so he's not in a wall.
    Sprite snorlax;
    snorlax.sheet = snorlaxSheet;
    snorlax.x = 4.5f;
    snorlax.y = 2.5f;
    snorlax.currentFrame = 0; 
    
    // Place Spiritomb directly South of the player's starting position
    Sprite spiritomb;
    spiritomb.sheet = spiritombSheet;
    spiritomb.x = 2.5f;
    spiritomb.y = 4.5f;
    spiritomb.currentFrame = 0; // Stays perfectly still at frame 0!

    float snorlaxFacingAngle = M_PI; // Make him face West (towards the spawn)
    float timeCounter = 0.0f;

    while (engine.IsRunning()) {
        engine.Update();

        player.HandleInput(engine, engine.GetDeltaTime(), map);

        player.UpdateCamera(camera);
        
        timeCounter += engine.GetDeltaTime();

        // 1. Figure out which row (direction) of the sprite sheet to use (0 to 7)
        int row = GetDirectionalRow(snorlax.x, snorlax.y, snorlaxFacingAngle, camera.pos.x, camera.pos.y);
        
        // 2. Animate between the 4 columns (frames) for that specific row
        int animOffset = (int)(timeCounter * 6.0f) % 4; 
        
        snorlax.currentFrame = (row * 4) + animOffset;

        raycaster.CalculateColumnGeometry(&camera, map);

        raycaster.CalculateRowGeometry(&camera);

        rasterizer.DrawWalls(raycaster.GetColBuffer(), &assets, &palette);

        rasterizer.DrawTexturedHorizon(raycaster.GetColBuffer(), raycaster.GetRowBuffer(), assets.GetTexture(8), assets.GetTexture(9));

        // Draw Snorlax right before the UI/FPS counter!
        if (snorlax.sheet != nullptr) {
            rasterizer.DrawSprite(snorlax, &camera, &raycaster);
        }

        // Draw Spiritomb! 
        if (spiritomb.sheet != nullptr) {
            rasterizer.DrawSprite(spiritomb, &camera, &raycaster);
        }

        rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xF800);
        engine.Present(rasterizer.GetPixels());
        engine.Wait();
    }

    return 0;
}
