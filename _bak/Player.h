#ifndef PLAYER_H
#define PLAYER_H

#include "../wolf_engine/src/Mathutil.h"

class Camera;
class Map;
class Engine;
class Input;

class Player {
private:
    Vector2 position;
    float angle;     
    float moveSpeed;
    float rotSpeed;
    float hitbox;

public:
    Player(Vector2 startPos, float startAngle, float hitbox);
    ~Player() = default;
    
    void HandleInput(Engine& engine, float dt, Map* map);
    void UpdateCamera(Camera& camera);
};

#endif
