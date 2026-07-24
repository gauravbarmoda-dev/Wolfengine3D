#include "Engine.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <iostream>

Engine::Engine() : window(nullptr), renderer(nullptr), isRunning(false), lastFrameTime(0), deltaTime(0.0f) {}

Engine::~Engine() {
    Quit();
}

bool Engine::Initialize(int width, int height, const char* title){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0){ 
        std::cerr << "SDL Failed to Initialize: " << SDL_GetError() << "\n";
        return false;
    }

    input.Init();

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    
    if(!window){
        std::cerr << "Failed to Create SDL window: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if(!renderer){
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << "\n";
    }
    
    isRunning = true;
    lastFrameTime = SDL_GetPerformanceCounter();
    perfFreq = 1.0f / (float)SDL_GetPerformanceFrequency();

    return true;
}

void Engine::Quit(){
    if(renderer){
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if(window){
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
    isRunning = false;
}

void Engine::Update(){
    input.Update();
    
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            isRunning = false;
        }
    }

    uint64_t currentFrameTime = SDL_GetPerformanceCounter();
    deltaTime = (float)(currentFrameTime - lastFrameTime) * perfFreq;
    lastFrameTime = currentFrameTime;
}

