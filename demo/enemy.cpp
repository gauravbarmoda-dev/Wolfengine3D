#include "enemy.h"

Enemy::Enemy(AssetMgr* assets, EnemyType type, float startX, float startY){
    sprite.x = startX;
    sprite.y = startY;
    sprite.currentFrame = 0;
    currentState = EnemyState::IDLE;

    if(type == EnemyType::GROUDON){
        animations.push_back(assets->LoadSpriteSheet("assets/sprites/entity/auromora/Idle-Anim.bmp", 48, 80, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("assets/sprites/entity/auromora/Walk-Anim.bmp", 48, 80, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("assets/sprites/entity/auromora/Rotate-Anim.bmp", 48, 80, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("assets/sprites/entity/auromora/Shoot-Anim.bmp", 56, 80, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("assets/sprites/entity/auromora/Charge-Anim.bmp", 48, 80, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("assets/sprites/entity/auromora/Hop-Anim.bmp", 48, 120, 0xF81F));
        animations.push_back(animations[0]);
    }

    if(!animations.empty()){
        sprite.sheet = animations[static_cast<int>(currentState)];
    }
}

void Enemy::Initialize(std::vector<SpriteSheet*> sheets, float startX, float startY){
    animations = sheets;
    sprite.x = startX;
    sprite.y = startY;

    if(!animations.empty()){
        sprite.sheet = animations[static_cast<int>(currentState)];
    }
}

void Enemy::Update(Camera* cam, float dt){
    (void)cam;
    // AI logic

    Animate(dt);
}

void Enemy::Animate(float dt){
    int state = static_cast<int>(currentState);

    if((size_t)state >= animations.size() || animations[state] == nullptr) return;

    animTimer += dt;
    const float TIME_PER_FRAME = 0.15f;

    if(animTimer >= TIME_PER_FRAME){
        animTimer -= TIME_PER_FRAME;
        animFrame++;

        if(animFrame >= animations[state]->numFrames){
            if(currentState == EnemyState::DEAD){
                animFrame = animations[state]->numFrames - 1;
            }
            else{
                animFrame = 0;
            }
        }
    }

    sprite.currentFrame = animFrame;
}

void Enemy::ChangeState(EnemyState newState){
    if(currentState == newState) return;
    
    currentState = newState;
    animFrame  = 0;
    animTimer = 0.0f;

    int state = static_cast<int>(currentState);
    if((size_t)state < animations.size() && animations[state] != nullptr){
        sprite.sheet = animations[state];
        sprite.currentFrame = 0;
    }
}
