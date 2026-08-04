#include "player.h"
#include "game.h"

Player::Player(Vector2 startPos, float startAngle, AssetMgr* assets) : 
    curPos(startPos), angle(startAngle), movSpeed(2.0f), rotSpeed(1400.0f), hitbox(0.3f),
    currState(PlayerState::IDLE), animTimer(0.0f), animSpd(0.15f){
    
    animations.push_back(assets->LoadSpriteSheet("assets/sprites/player/Idle-Anim.bmp", 18, 20, 0xF81F));
    animations.push_back(assets->LoadSpriteSheet("assets/sprites/player/Walk-Anim.bmp", 18, 20, 0xF81F));
    animations.push_back(assets->LoadSpriteSheet("assets/sprites/player/Eat-Anim.bmp", 18, 4, 0xF81F));
    animations.push_back(assets->LoadSpriteSheet("assets/sprites/player/Hurt-Anim.bmp", 22, 26, 0xF81F));

    sprite.sheet = animations[static_cast<int>(currState)];
    sprite.currentFrame = 0;
    sprite.x = curPos.x;
    sprite.y = curPos.y;

    attacks[static_cast<int>(AttackID::TACKLE)] = {
        20, 1.0f, 0.0f, 7.0f, assets->LoadSpriteSheet("assets/sprites/player/Tackle-Anim.bmp", 18, 20, 0xF81F)
    };
    attacks[static_cast<int>(AttackID::HEADBUTT)] = {
        20, 1.0f, 0.0f, 7.0f, assets->LoadSpriteSheet("assets/sprites/player/Headbutt-Anim.bmp", 20, 20, 0xF81F)
    };
    attacks[static_cast<int>(AttackID::BUBBLE)] = {
        20, 1.0f, 0.0f, 7.0f, assets->LoadSpriteSheet("assets/sprites/player/Bubble-Anim.bmp", 18, 22, 0xF81F)
    };
    attacks[static_cast<int>(AttackID::FLY)] = {
        20, 1.0f, 0.0f, 7.0f, assets->LoadSpriteSheet("assets/sprites/player/Fly-Anim.bmp", 20, 18, 0xF81F)
    };

    attackX = AttackID::TACKLE;
    attackY = AttackID::HEADBUTT;
}

Player::~Player(){
}

void Player::Update(Camera* cam, Engine& engine, Map* map, float dt){
    Input& input = engine.GetInput();

    //Close Game
    if(input.isGamepadDown(Gamepad::Select) && input.isGamepadDown(Gamepad::Start)) engine.Stop();

    // Cooldown attacks;
    for(int i = 0; i < static_cast<int>(AttackID::COUNT); i++){
        if(attacks[i].currCooldown > 0.0f){
            attacks[i].currCooldown -= dt;
        }
    }

    Vector2 movDir(fcos(angle), fsin(angle));
    float movStep = movSpeed * dt;
    float rotStep = rotSpeed * dt;

    if(currState == PlayerState::ATTACKING){
        //Attack Movement logic
    }

    Vector2 newPos = curPos;
    Vector2 rightDir(-movDir.y, movDir.x);
    
    // Movement
    if(input.isKeyDown(Keys::W) || input.isGamepadDown(Gamepad::DpadUp))    {newPos = newPos + movDir * movStep;}
    if(input.isKeyDown(Keys::S) || input.isGamepadDown(Gamepad::DpadDown))  {newPos = newPos - movDir * movStep;}
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
    
    // State Logic
    bool isMoving = (input.isKeyDown(Keys::W) || input.isKeyDown(Keys::A) || input.isKeyDown(Keys::S) || input.isKeyDown(Keys::D) ||
                     input.isGamepadDown(Gamepad::DpadUp) || input.isGamepadDown(Gamepad::DpadLeft) ||
                     input.isGamepadDown(Gamepad::DpadDown) || input.isGamepadDown(Gamepad::DpadRight)
    );

    PlayerState nextState = isMoving ? PlayerState::WALK : PlayerState::IDLE;

    //Attack Logic
    if(currState == PlayerState::ATTACKING){
        nextState = PlayerState::ATTACKING;

        if(sprite.currentFrame == sprite.sheet->numFrames - 1 && (animTimer + dt) >= animSpd){
            nextState = isMoving ? PlayerState::WALK : PlayerState::IDLE;
        }
    }
    else{
        if((input.isKeyPressed(Keys::R) || input.isGamepadPressed(Gamepad::X)) && attacks[static_cast<int>(attackX)].currCooldown <= 0.0f){
            nextState = PlayerState::ATTACKING;
            activeAtk = attackX;
            attacks[static_cast<int>(attackX)].currCooldown = attacks[static_cast<int>(attackX)].maxCooldown;
        }
        else if((input.isKeyPressed(Keys::F) || input.isGamepadPressed(Gamepad::Y)) && attacks[static_cast<int>(attackY)].currCooldown <= 0.0f){
            nextState = PlayerState::ATTACKING;
            activeAtk = attackY;
            attacks[static_cast<int>(attackY)].currCooldown = attacks[static_cast<int>(attackY)].maxCooldown;
        }
    }

    // State Change
    if(currState != nextState){
        currState = nextState;   
        if(currState == PlayerState::ATTACKING){
            sprite.sheet = attacks[static_cast<int>(activeAtk)].animation;
        }
        else{
            sprite.sheet = animations[static_cast<int>(currState)];
        }
        sprite.currentFrame = 0;
        animTimer = 0.0f;
    }

    Animate(dt);

    // 3rd Person
    sprite.x = curPos.x;
    sprite.y = curPos.y;

    Vector2 camTarget = curPos - (movDir * 1.5f);
    if(map->GetWorldTile(camTarget.x, camTarget.y) == 0){
        cam->pos = camTarget;
    }
    else{
        cam->pos = curPos - (movDir * 0.4f);
    }

    // Camera Update
    cam->absAngle = angle;
    cam->Update();
}

void Player::Animate(float dt){
    if(sprite.sheet == nullptr) return;

    animTimer += dt;
    if(animTimer >= animSpd){
        animTimer -= animSpd;
        sprite.currentFrame++;

        if(sprite.currentFrame >= sprite.sheet->numFrames){
            sprite.currentFrame = 0;
        }
    }
}
