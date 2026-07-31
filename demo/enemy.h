#ifndef ENEMY_H
#define ENEMY_H

#include "../engine/shlong.h"

class Player;

class Enemy{
private:
    Vector2 curPos;
    Sprite sprite;
    float angle;

public:
    ~Enemy() = default;

    void Update(Camera* cam);

    Sprite* GetSprite(){return &sprite;}
};

#endif
