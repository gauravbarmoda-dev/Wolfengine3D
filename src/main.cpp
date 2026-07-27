#include "../wolf_engine/src/Rasterizer.h"
#include "../wolf_engine/src/Raycaster.h"
#include "../wolf_engine/src/AssetMgr.h"
#include "../wolf_engine/src/Palette.h"
#include "../wolf_engine/src/Camera.h"
#include "../wolf_engine/src/Engine.h"
#include "../wolf_engine/src/Map.h"
#include "Player.h"

#define SCREEN_HEIGHT   480
#define SCREEN_WIDTH    640

const uint16_t BROWN  = 0x4228;
const uint16_t GRAY = 0x5182;

int main() {
    Raycaster  raycaster(SCREEN_WIDTH, SCREEN_HEIGHT);
    Rasterizer rasterizer;
    Palette    palette;
    Engine     engine;
    AssetMgr   assets;

    if (!engine.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Wolf Engine")) return -1;
    rasterizer.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);
    engine.TargetFPS(1000);

    Map* map = assets.LoadMap("assets/maps/level1.map", 16);
    
    assets.LoadTexture(1, "assets/sprites/brick_wall.bmp");
    assets.LoadTexture(2, "assets/sprites/cement_wall.bmp");
    assets.LoadTexture(3, "assets/sprites/wooden_door.bmp");

    Camera camera;
    Player player(Vector2(1.5f, 1.5f), 0.0f, 0.2f);

    while (engine.IsRunning()) {  
        engine.Update();

        player.HandleInput(engine, engine.GetDeltaTime(), map);
        player.UpdateCamera(camera);
        rasterizer.ClearHorizon(GRAY, BROWN);
        raycaster.Render(&camera, map, &palette, &rasterizer, &assets);
        rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xFFFF);
        
        engine.Present(rasterizer.GetPixels());
        engine.Wait();
    }

    return 0;
}
