#include "../wolf_engine/src/Rasterizer.h"
#include "../wolf_engine/src/Raycaster.h"
#include "../wolf_engine/src/AssetMgr.h"
#include "../wolf_engine/src/Palette.h"
#include "../wolf_engine/src/Camera.h"
#include "../wolf_engine/src/Engine.h"
#include "../wolf_engine/src/Map.h"
#include "Player.h"

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

    Camera camera;
    Player player(Vector2(2.5f, 2.5f), 0.0f, 0.2f);

    while (engine.IsRunning()) {
        engine.Update();

        player.HandleInput(engine, engine.GetDeltaTime(), map);

        player.UpdateCamera(camera);

        raycaster.CalculateColumnGeometry(&camera, map);

        raycaster.CalculateRowGeometry(&camera);

        rasterizer.DrawWalls(raycaster.GetColBuffer(), &assets, &palette);

        rasterizer.DrawTexturedHorizon(raycaster.GetColBuffer(), raycaster.GetRowBuffer(), assets.GetTexture(8), assets.GetTexture(9));

        rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xF800);
        engine.Present(rasterizer.GetPixels());
        engine.Wait();
    }

    return 0;
}
