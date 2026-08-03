#include "weapon.h"

Weapon::Weapon(AssetMgr* assets, WeaponType type) : 
    weaponType(type), currentState(WeaponState::IDLE),
    cooldown(0.0f), animTimer(0.0f), animFrame(0), animSpeed(0.1f){

    if(weaponType == WeaponType::PISTOL){
        damage = 25;
        fireRate = 0.5f;
        animSpeed = 0.1f;
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
    }
    else if(weaponType == WeaponType::AR){
        damage = 15;
        fireRate = 0.1f;
        animSpeed = 0.1f;
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
    }
    else if(weaponType == WeaponType::SHOTGUN){
        damage = 80;
        fireRate = 1.2f;
        animSpeed = 0.15f;
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
        animations.push_back(assets->LoadSpriteSheet("xyz", 1, 1, 0xF81F));
    }
}

void Weapon::Update(float dt){
    
}

void Weapon::Animate(float dt){
    if(cooldown > 0.0f){
        cooldown -= dt;    
    } 

    SpriteSheet* currSheet = GetCurrentSheet();
    if(!currSheet) return;

    if(currentState == WeaponState::FIRING){
        animTimer += dt;
        if(animTimer >= animSpeed){
            animTimer -= animSpeed;
            animFrame++;

            if(animFrame >= currSheet->numFrames){
                animFrame = 0;
                currentState = WeaponState::IDLE;
            }
        }
    }
    else{
        animFrame = 0;
    }
}

bool Weapon::TryFire(){
    if(currentState == WeaponState::IDLE && cooldown <= 0.0f){
        currentState = WeaponState::FIRING;
        cooldown = fireRate;
        animFrame = 0;
        animTimer = 0.0f;
        return true;
    }
    return false;
}

SpriteSheet* Weapon::GetCurrentSheet() const {
    int stateIdx = static_cast<int>(currentState);
    if((size_t)stateIdx < animations.size()){
        return animations[stateIdx];
    }
    return nullptr;
}
