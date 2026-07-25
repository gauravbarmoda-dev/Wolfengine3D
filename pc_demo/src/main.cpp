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

void HandleInput(Engine& engine, Camera& camera, Map& map, float dt){
    float mvSpeed = 3.0f * dt;
    int rotSpeed =  dt * 1400.0f;     //2x

    auto& input = engine.GetInput();

    if(input.isKeyDown(Keys::W)) camera.Move(mvSpeed, &map); 
    if(input.isKeyDown(Keys::S)) camera.Move(-mvSpeed, &map);

    if(input.isKeyDown(Keys::A)) camera.Strafe(-mvSpeed, &map);    
    if(input.isKeyDown(Keys::D)) camera.Strafe(mvSpeed, &map);

    if(input.isKeyDown(Keys::Q)) camera.Rotate(-rotSpeed);    
    if(input.isKeyDown(Keys::E)) camera.Rotate(rotSpeed);    
    
    if(input.isKeyDown(Keys::R)) engine.Quit();    
}

int main(){
    Raycaster  raycaster(SCREEN_WIDTH, SCREEN_HEIGHT);
    Rasterizer rasterizer;
    Camera     camera;
    Engine     engine;
    Map        map("pc_demo/maps/level2.map", 16);

    engine.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Test");
    rasterizer.Initialize(engine.GetRenderer(), SCREEN_WIDTH, SCREEN_HEIGHT);

    engine.TargetFPS(100);

    while(engine.IsRunning()){  
        engine.Update();

        HandleInput(engine, camera, map, engine.GetDeltaTime());

        rasterizer.ClearHorizon(GRAY, BROWN);

        raycaster.Render(&camera, &map, &rasterizer);

        rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xFFFF);

        rasterizer.Present(engine.GetRenderer());

        engine.Wait();
    }

    return 0;
}
