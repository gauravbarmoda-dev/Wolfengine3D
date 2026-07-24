#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL_gamecontroller.h>
#include <vector>

enum class Keys{
    W, A, S, D, E, Q
};

enum class Gamepad{
    DpadUp, DpadDown, DpadLeft, DpadRight,
    L1, R1, A, B, X, Y, Start, Select
};

class Input{
private:
    std::vector<unsigned char> previousKeyState;
    std::vector<unsigned char> prevGamepadState;

    SDL_GameController* controller;
    const unsigned char* sdlKey;
    int numKey;

public:
    Input();
    ~Input();

    void Init();
    void Update();
    
    bool isKeyDown(Keys key);
    bool isKeyPressed(Keys key);

    bool isGamepadDown(Gamepad button);
    bool isGamepadPressed(Gamepad button);
};

#endif
