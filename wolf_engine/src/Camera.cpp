#include "Camera.h"
#include "Map.h"

Camera::Camera() : pos(1.5f, 1.5f), absAngle(0.0f), fov(0.66f) {
    Update();
}

Camera::Camera(Vector2 startPos, float startAngle, float f) {
    pos = startPos;
    absAngle = startAngle;
    fov = f;
    Update();
}

void Camera::Update(){
    dir.x = fcos((int)absAngle);
    dir.y = fsin((int)absAngle);
    plane.x = -dir.y * fov;
    plane.y =  dir.x * fov;
}

void Camera::Rotate(float angleDelta){
    absAngle += angleDelta;
    Update();
}

void Camera::Move(float distance, Map* map){
    float moveX = dir.x * distance;
    float moveY = dir.y * distance;

    float padX = (moveX > 0) ? 0.2f : -0.2f;
    float padY = (moveY > 0) ? 0.2f : -0.2f;

    if(map->GetWorldTile(pos.x + moveX + padX, pos.y) == 0) pos.x += moveX;
    if(map->GetWorldTile(pos.x, pos.y + moveY + padY) == 0) pos.y += moveY;
}

void Camera::Strafe(float distance, Map* map){
    float moveX = -dir.y * distance;
    float moveY = dir.x * distance;

    float padX = (moveX > 0) ? 0.2f : -0.2f;
    float padY = (moveY > 0) ? 0.2f : -0.2f;
    
    if(map->GetWorldTile(pos.x + moveX + padX, pos.y) == 0) pos.x += moveX;
    if(map->GetWorldTile(pos.x, pos.y + moveY + padY) == 0) pos.y += moveY;
}
