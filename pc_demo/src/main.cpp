#include "../../wolf_engine/src/Rasterizer.h"
#include "../../wolf_engine/src/Raycaster.h"
#include "../../wolf_engine/src/Camera.h"
#include "../../wolf_engine/src/Engine.h"
#include "../../wolf_engine/src/Map.h"
#include <cstdint>
#include <iostream>
#include <SDL2/SDL.h>

#define SCREEN_HEIGHT   480
#define SCREEN_WIDTH    640

const uint16_t GRAY  = 0x4228;
const uint16_t BROWN = 0x5182;
const uint16_t RED   = 0xF800;

void HandleInput(Engine& engine, Camera& camera, Map& map){
    float mvSpeed = 3.0f * engine.GetDeltaTime();
    int rotSpeed = (int)(1303.79f * engine.GetDeltaTime());     //2x

    auto& input = engine.GetInput();

    if(input.isKeyDown(Keys::W)) camera.Move(mvSpeed, &map); 
    if(input.isKeyDown(Keys::S)) camera.Move(-mvSpeed, &map);

    if(input.isKeyDown(Keys::A)) camera.Strafe(-mvSpeed, &map);    
    if(input.isKeyDown(Keys::D)) camera.Strafe(mvSpeed, &map);

    if(input.isKeyDown(Keys::Q)) camera.Rotate(-rotSpeed);    
    if(input.isKeyDown(Keys::E)) camera.Rotate(rotSpeed);    
}

int main(){
    Raycaster  raycaster(SCREEN_WIDTH, SCREEN_HEIGHT);
    Rasterizer rasterizer;
    Camera     camera;
    Engine     engine;
    Map        map("wolf_game_demo/maps/level1.map", 64);

    engine.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Test");
    rasterizer.Initialize(engine.GetRenderer(), SCREEN_WIDTH, SCREEN_HEIGHT);
    
    while(engine.IsRunning()){
        engine.Update();

        HandleInput(engine, camera, map);

        rasterizer.ClearHorizon(GRAY, BROWN);

        raycaster.Render(&camera, &map, &rasterizer);

        rasterizer.Present(engine.GetRenderer());
    }

    return 0;
}
