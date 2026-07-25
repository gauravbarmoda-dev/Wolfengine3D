#ifndef ENGINE_H
#define ENGINE_H

#include "Input.h"

struct SDL_Window;
struct SDL_Renderer;

class Engine{
private:
    SDL_Window*   window;
    SDL_Renderer* renderer;

    Input input;

    bool isRunning;

    uint64_t lastFrameTime;
    float deltaTime;
    float performanceFreq;

    float targetFrameTime = 0.0f;

    float fpsTimer = 0.0f;
    int frameCount = 0;
    int curFPS     = 0;

public:
    Engine();
    ~Engine();

    bool Initialize(int width, int height, const char* title);
    void Update();
    void Wait();
    void Quit();

    void TargetFPS(int fps) {targetFrameTime = (fps > 0) ? (1.0f/fps) : 0;}

    int GetFPS()                {return curFPS;}
    bool IsRunning()            {return isRunning;}
    float GetDeltaTime()        {return deltaTime;}
    Input& GetInput()           {return input;}
    SDL_Renderer* GetRenderer() {return renderer;}
};
#endif
