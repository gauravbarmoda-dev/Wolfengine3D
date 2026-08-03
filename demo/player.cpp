#include "player.h"
#include "weapon.h"

Player::Player(Vector2 startPos, float startAngle, AssetMgr* assets) : 
    curPos(startPos), angle(startAngle), movSpeed(2.0f), rotSpeed(1400.0f), hitbox(0.3f) {
    equippedWeapon = new Weapon(assets, WeaponType::PISTOL);
}

Player::~Player(){
    if(equippedWeapon) delete equippedWeapon;
}

void Player::Update(Camera* cam, Engine& engine, Map* map, float dt){
    Input& input = engine.GetInput();

    //Close Game
    if(input.isGamepadDown(Gamepad::Select) && input.isGamepadDown(Gamepad::Start)) engine.Stop();

    float movStep = movSpeed * dt;
    float rotStep = rotSpeed * dt;

    Vector2 newPos = curPos;
    Vector2 movDir(fcos(angle), fsin(angle));
    Vector2 rightDir(-movDir.y, movDir.x);
    
    // Vertical
    if(input.isKeyDown(Keys::W) || input.isGamepadDown(Gamepad::DpadUp))    {newPos = newPos + movDir * movStep;}
    if(input.isKeyDown(Keys::S) || input.isGamepadDown(Gamepad::DpadDown))  {newPos = newPos - movDir * movStep;}

    // Horizontal
    if(input.isKeyDown(Keys::A) || input.isGamepadDown(Gamepad::DpadLeft))  {newPos = newPos - rightDir * movStep;}
    if(input.isKeyDown(Keys::D) || input.isGamepadDown(Gamepad::DpadRight)) {newPos = newPos + rightDir * movStep;}

    // Camera Rotation
    if(input.isKeyDown(Keys::Q) || input.isGamepadDown(Gamepad::L1)){angle -= rotStep;}
    if(input.isKeyDown(Keys::E) || input.isGamepadDown(Gamepad::R1)){angle += rotStep;}

    // HitBox Check
    float checkX = (newPos.x > curPos.x) ? (hitbox) : (-hitbox);
    float checkY = (newPos.y > curPos.y) ? (hitbox) : (-hitbox);

    // Wall Collision Check
    if(map->GetWorldTile(newPos.x + checkX, curPos.y - hitbox) == 0 &&
       map->GetWorldTile(newPos.x + checkX, curPos.y + hitbox) == 0)
    {curPos.x = newPos.x;}
    if(map->GetWorldTile(curPos.x - hitbox, newPos.y + checkY) == 0 &&
       map->GetWorldTile(curPos.x + hitbox, newPos.y + checkY) == 0)
    {curPos.y = newPos.y;}
    
    // Camera Update
    cam->pos = curPos;
    cam->absAngle = angle;
    cam->Update();

    // Weapon 
    equippedWeapon->Update(dt);
    if(input.isKeyPressed(Keys::R) || input.isGamepadPressed(Gamepad::A)){
        if(equippedWeapon->TryFire()){
            // raycaster logic
        }
    }
}
