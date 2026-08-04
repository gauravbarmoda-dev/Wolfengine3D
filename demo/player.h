#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/shlong.h"
#include <vector>

enum class AttackID{
    TACKLE,
    HEADBUTT,
    BUBBLE,
    FLY,
    COUNT
};

enum class PlayerState{
    IDLE,
    WALK,
    EAT,
    HURT,
    ATTACKING,
};

struct Attack{
    int damage;
    float maxCooldown;
    float currCooldown;
    float range;
    SpriteSheet* animation;
};

class Player{
private:
    Vector2 curPos;
    float angle;
    float movSpeed;
    float rotSpeed;
    float hitbox;

    Sprite sprite;
    std::vector<SpriteSheet*>animations;

    PlayerState currState;
    float animTimer;
    float animSpd;
    
    Attack attacks[static_cast<int>(AttackID::COUNT)];
    AttackID attackX; 
    AttackID attackY;
    AttackID activeAtk;

public:
    Player(Vector2 startPos, float startAngle, AssetMgr* assets);
    ~Player();

    void Update(Camera* cam, Engine& engine, Map* map, float dt);

    Sprite* GetSprite() {return &sprite;}

private:
    void Animate(float dt);
};

#endif
