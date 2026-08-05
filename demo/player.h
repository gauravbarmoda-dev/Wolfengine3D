#ifndef PLAYER_H
#define PLAYER_H

#include "../engine/shlong.h"
#include <vector>

class Entity;

enum class AttackID{
    TACKLE,
    HEADBUTT,
    BUBBLE,
    COUNT
};

enum class PlayerState{
    IDLE,
    WALK,
    EAT,
    HURT,
    JUMP,
    FLY,
    ATTACKING
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
    float jumpTimer;
    
    Attack attacks[static_cast<int>(AttackID::COUNT)];
    AttackID attackX; 
    AttackID attackY;
    AttackID activeAtk;

    int currDir = 4;
    int visualDir = 4;
    float turnTimer = 0.0f;

    float flyTimer = 0.0f;

public:
    Player(Vector2 startPos, float startAngle, AssetMgr* assets);
    ~Player();

    void Update(Entity* manager, Camera* cam, Engine& engine, Map* map, float dt);

    Sprite* GetSprite() {return &sprite;}
    float GetHitbox() const {return hitbox;}

private:
    void Animate(float dt);

    void UpdateJump(float dt, bool isMoving, PlayerState& nextState);
    void UpdateAttack(Entity* manager, Map* map, float dt, bool isMoving, PlayerState& nextState);
    void UpdateFly(float dt, bool isMoving, PlayerState& nextState);
};

#endif
