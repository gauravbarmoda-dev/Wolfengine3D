#ifndef WEAPONS_H
#define WEAPONS_H

#include "../engine/shlong.h"

enum class WeaponState{
    IDLE,
    FIRING,
    RELOADING
};

enum class WeaponType{
    PISTOL,
    AR,
    SHOTGUN,
};

class Weapon{
private:
    WeaponType weaponType;
    WeaponState currentState;
    std::vector<SpriteSheet*> animations;

    int damage;
    float fireRate;
    float cooldown;

    int animFrame;
    float animTimer;
    float animSpeed;

public:
    Weapon(AssetMgr* assets, WeaponType type);
    ~Weapon() = default;

    void Update(float dt);

    WeaponState GetState() const {return currentState;}
    SpriteSheet* GetCurrentSheet() const;
    int GetCurrentFrame() const {return animFrame;}
    int GetDamage() const {return damage;}
    
    bool TryFire();
private:
    void Animate(float dt);
};

#endif
