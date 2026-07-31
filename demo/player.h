#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/shlong.h"

class Player{
private:
    Vector2 curPos;
    float angle;
    float movSpeed;
    float rotSpeed;
    float hitbox;

public:

    Player(Vector2 startPos, float startAngle);
    ~Player() = default;

    void Update(Camera* cam, Engine& engine, Map* map, float dt);
};

#endif
