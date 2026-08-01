#include "game.h"
#include "enemy.h"
#include "entity_mgr.h"
#include "player.h"
#include "world.h"

Game::Game() : raycaster(nullptr), player(nullptr), world(nullptr), level(0) {}

Game::~Game(){
    if(raycaster) delete raycaster; 
    if(player)    delete player;
    if(world)     delete world;
}

bool Game::Initialize(int width, int height, const char* title){
    scrHeight = height;
    scrWidth  = width;

    if(!engine.Initialize(scrWidth, scrHeight, title)) return false;
    engine.TargetFPS(100);

    rasterizer.Initialize(width, height);
    
    raycaster = new Raycaster(width, height);

    player = new Player(Vector2(2.5f, 2.5f), 0.0f);
    
    world = new World();
    world->Initialize(&assets);

    world->LoadLevel(level);

    entity = new Entity();

    Enemy* alomora = new Enemy(&assets, EnemyType::GROUDON, 1.5f, 1.5f);
    entity->AddEnemy(alomora);

    return true;
}

void Game::Run(){
    while(engine.IsRunning()){
        Update();
        Render();
        Present();
    }
}

void Game::Update(){
    engine.Update();
    
    player->Update(&camera, engine, world->GetMap(), engine.GetDeltaTime());

    entity->Update(&camera, engine.GetDeltaTime());
}

void Game::Render(){
    raycaster->CalculateColumnGeometry(&camera, world->GetMap());
    raycaster->CalculateRowGeometry(&camera);

    rasterizer.DrawTexturedHorizon(
        raycaster->GetColBuffer(),
        raycaster->GetRowBuffer(),
        world->Getfloor()[0],
        world->GetCeil()[0]
    );

    rasterizer.DrawWalls(raycaster->GetColBuffer(), &assets, &palette);

    entity->QueueSprites(&rasterizer);
    rasterizer.DrawSprites(&camera, raycaster);

    rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xF800);
}

void Game::Present(){
    engine.Present(rasterizer.GetPixels());
    engine.Wait();
}
