#include "../../wolf_engine/src/Rasterizer.h"
#include "../../wolf_engine/src/Raycaster.h"
#include "../../wolf_engine/src/Palette.h"
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

int main(){
    Raycaster  raycaster(SCREEN_WIDTH, SCREEN_HEIGHT);
    Rasterizer rasterizer;
    Palette    palette;
    Camera     camera;
    Engine     engine;
    Map        map("pc_demo/maps/level2.map", 16);

    engine.Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Test");
    rasterizer.Initialize(engine.GetRenderer(), SCREEN_WIDTH, SCREEN_HEIGHT);

    engine.TargetFPS(100);

    while(engine.IsRunning()){  
        engine.Update();

        rasterizer.ClearHorizon(GRAY, BROWN);

        raycaster.Render(&camera, &map, &palette, &rasterizer);

        rasterizer.DrawFPS(engine.GetFPS(), 10, 10, 0xFFFF);

        rasterizer.Present(engine.GetRenderer());

        engine.Wait();
    }

    return 0;
}
