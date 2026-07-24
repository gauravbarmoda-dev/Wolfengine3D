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
    float perfFreq;

public:
    Engine();
    ~Engine();

    bool Initialize(int width, int height, const char* title);
    void Update();
    void Quit();

    bool IsRunning()            {return isRunning;}
    float GetDeltaTime()        {return deltaTime;}
    Input& GetInput()           {return input;}
    SDL_Renderer* GetRenderer() {return renderer;}
};
#endif
