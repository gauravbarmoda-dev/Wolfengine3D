#include "game.h"
#include "enemy.h"
#include "entity_mgr.h"
#include "player.h"
#include "world.h"

Game::Game() : raycaster(nullptr), entity(nullptr), world(nullptr), player(nullptr), level(0) {}

Game::~Game(){
    if(raycaster) delete raycaster; 
    if(player)    delete player;
    if(world)     delete world;
    if(entity)    delete entity;
}

bool Game::Initialize(int width, int height, const char* title){
    scrHeight = height;
    scrWidth  = width;

    if(!engine.Initialize(scrWidth, scrHeight, title)) return false;
    engine.TargetFPS(1000);

    rasterizer.Initialize(width, height);
    
    raycaster = new Raycaster(width, height);

    player = new Player(Vector2(2.5f, 2.5f), 0.0f, &assets);
    
    world = new World();
    world->Initialize(&assets);

    world->LoadLevel(level);

    entity = new Entity();
    Enemy* blastoice = new Enemy(&assets, EnemyType::BLASTOICE, 3.5f, 3.5f);
    entity->AddEnemy(blastoice);

    Enemy* charizard = new Enemy(&assets, EnemyType::CHARIZARD, 1.5f, 1.5f);
    entity->AddEnemy(charizard);

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

    entity->Update(&camera, world->GetMap(), engine.GetDeltaTime());
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
    rasterizer.QueueSprite(player->GetSprite());
    rasterizer.DrawSprites(&camera, raycaster);

    rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xF800);
}

void Game::Present(){
    engine.Present(rasterizer.GetPixels());
    engine.Wait();
}
