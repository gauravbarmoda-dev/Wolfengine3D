#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/shlong.h"
#include "weapon.h"

class Player{
private:
    Vector2 curPos;
    float angle;
    float movSpeed;
    float rotSpeed;
    float hitbox;

    Weapon* equippedWeapon;

public:
    Player(Vector2 startPos, float startAngle, AssetMgr* assets);
    ~Player();

    void Update(Camera* cam, Engine& engine, Map* map, float dt);

    Weapon* GetWeapon() {return equippedWeapon;}
};

#endif
