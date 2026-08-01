#ifndef ENEMY_H
#define ENEMY_H

#include "../engine/shlong.h"
#include <vector>

class Player;

enum class EnemyState{
    IDLE    = 0,
    WALK    = 1,
    ROTATE  = 2,
    SHOOT   = 3,
    CHARGE  = 4,
    HOP     = 5,
    DEAD    = 6
};

enum class EnemyType{
    GROUDON
};

class Enemy{
private:
    Sprite sprite;
    EnemyState currentState;
    std::vector<SpriteSheet*> animations;
    float animTimer  = 0.0f;
    int animFrame    = 0;
    
public:
    Enemy(AssetMgr* assets, EnemyType type, float startX, float startY);
    ~Enemy() = default;

    void Initialize(std::vector<SpriteSheet*> sheets, float startX, float startY);

    void Update(Camera* cam, float dt);

    Sprite* GetSprite(){return &sprite;}

private:
    void Animate(float dt);
    void ChangeState(EnemyState newState);
};

#endif
