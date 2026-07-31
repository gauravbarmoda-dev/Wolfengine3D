#include "../engine/shlong.h"
#include "player.h"
#include "enemy.h"

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480
#define NO_OF_MAPS      2
#define NO_OF_ENEMIES   1

uint16_t BROWN = 0x5A06;
uint16_t GRAY  = 0xAD55;

Map*   maps[NO_OF_MAPS];
Enemy  enemies[NO_OF_ENEMIES];

void LoadAssets(AssetMgr* assets){
    // Maps
    maps[0] = assets->LoadMap("assets/maps/level2.map", 32);
    maps[1] = assets->LoadMap("assets/maps/level1.map", 16);

    // Walls
    assets->LoadTexture(1, "assets/sprites/walls/stone_wall.bmp");
    assets->LoadTexture(2, "assets/sprites/walls/brick_wall1.bmp");
    assets->LoadTexture(3, "assets/sprites/walls/brick_wall2.bmp");
    assets->LoadTexture(4, "assets/sprites/walls/brick_wall3.bmp");
    assets->LoadTexture(5, "assets/sprites/walls/brick_muf_wall.bmp");

    // Floor
    assets->LoadTexture(8, "assets/sprites/floor/mud_grass_floor.bmp");

    // Ceil
    assets->LoadTexture(9, "assets/sprites/ceil/mud_ceiling.bmp");

    // Sprites
    SpriteSheet* groudon = assets->LoadSpriteSheet("assets/sprites/entity/groudon/Idle-Anim.bmp", 64, 80, 0xF81F);
    enemies[0].GetSprite()->sheet = groudon;
}

int main(){

    Rasterizer rasterizer;
    rasterizer.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

    Raycaster raycaster(SCREEN_WIDTH, SCREEN_HEIGHT);

    Engine engine;
    if(!engine.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Demo")){
        return -1;
    }
    engine.TargetFPS(100);

    AssetMgr assets;
    LoadAssets(&assets);

    Palette palette;
    Camera camera;
    Player player(Vector2(2.5f, 2.5f), 0.0f);

    enemies[0].GetSprite()->x = 8.5f;
    enemies[0].GetSprite()->y = 8.5f;
    enemies[0].GetSprite()->currentFrame = 0;

    while(engine.IsRunning()){
        engine.Update();
        enemies[0].Update(&camera);
        player.Update(&camera, engine, maps[1], engine.GetDeltaTime());

        for(int i = 0; i < NO_OF_ENEMIES; i++){
            rasterizer.QueueSprite(enemies[i].GetSprite());
        }

        raycaster.CalculateColumnGeometry(&camera, maps[1]);
        raycaster.CalculateRowGeometry(&camera);

        rasterizer.DrawTexturedHorizon(raycaster.GetColBuffer(), raycaster.GetRowBuffer(), assets.GetTexture(8), assets.GetTexture(9));
        rasterizer.DrawWalls(raycaster.GetColBuffer(), &assets, &palette);

        rasterizer.DrawSprites(&camera, &raycaster);

        rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xF800);
        engine.Present(rasterizer.GetPixels());
        engine.Wait();
    }

    return 0;
}
