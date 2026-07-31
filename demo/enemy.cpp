#include "enemy.h"

void Enemy::Update(Camera* cam){
    float dx = cam->pos.x - sprite.x;
    float dy = cam->pos.y - sprite.y;

    float angleToPlayerRad = std::atan2(dy, dx);
    int angleToPlayer = (angleToPlayerRad * 651.8986f);

    int diff = angleToPlayer - (int)angle;
    diff = (diff % 4096 + 4096) % 4096;

    int direction = ((diff + 256) % 4096) / 512;
    direction = (8 - direction) % 8;

    sprite.currentFrame = (direction * 14) + 0;
}
