#include "Player.h"
#include "../wolf_engine/src/Map.h"
#include "../wolf_engine/src/Input.h"
#include "../wolf_engine/src/Engine.h"
#include "../wolf_engine/src/Camera.h"

Player::Player(Vector2 startPos, float startAngle, float hitbox) 
    : position(startPos), angle(startAngle), moveSpeed(4.0f), rotSpeed(1400.0f), hitbox(0.2f) {}

void Player::HandleInput(Engine& engine, float dt, Map* map) {
    if (engine.GetInput().isGamepadDown(Gamepad::Select) && engine.GetInput().isGamepadDown(Gamepad::Start)) {
        engine.Stop();
    }

    if (!map) return;

    Input& input = engine.GetInput();

    float moveStep = moveSpeed * dt;
    float rotStep  = rotSpeed * dt;

    if (input.isKeyDown(Keys::Q) || input.isGamepadDown(Gamepad::L1)) angle -= rotStep;
    if (input.isKeyDown(Keys::E) || input.isGamepadDown(Gamepad::R1)) angle += rotStep;
    
    if (angle < 0) angle += TRIG_LUT_SIZE;
    if (angle >= TRIG_LUT_SIZE) angle -= TRIG_LUT_SIZE;

    Vector2 moveDir(fcos((int)angle), fsin((int)angle));
    Vector2 rightDir(-moveDir.y, moveDir.x);
    Vector2 moveVec(0.0f, 0.0f);

    if (input.isKeyDown(Keys::W) || input.isGamepadDown(Gamepad::DpadUp))   moveVec = moveVec + moveDir * moveStep;
    if (input.isKeyDown(Keys::S) || input.isGamepadDown(Gamepad::DpadDown)) moveVec = moveVec - moveDir * moveStep;
    
    if (input.isKeyDown(Keys::A) || input.isGamepadDown(Gamepad::DpadLeft))  moveVec = moveVec - rightDir * moveStep;
    if (input.isKeyDown(Keys::D) || input.isGamepadDown(Gamepad::DpadRight)) moveVec = moveVec + rightDir * moveStep;

    float checkX = (moveVec.x > 0) ? hitbox : -hitbox;
    float checkY = (moveVec.y > 0) ? hitbox : -hitbox;

    if (map->GetWorldTile(position.x + moveVec.x + checkX, position.y - hitbox) == 0 &&
        map->GetWorldTile(position.x + moveVec.x + checkX, position.y + hitbox) == 0) {
        position.x += moveVec.x;
    }
    
    if (map->GetWorldTile(position.x - hitbox, position.y + moveVec.y + checkY) == 0 &&
        map->GetWorldTile(position.x + hitbox, position.y + moveVec.y + checkY) == 0) {
        position.y += moveVec.y;
    }
}

void Player::UpdateCamera(Camera& camera) {
    camera.pos = position;
    camera.absAngle = angle;
    camera.Update();
}
