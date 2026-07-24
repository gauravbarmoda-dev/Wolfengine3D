#include "Camera.h"
#include "Map.h"

Camera::Camera() : pos(1.5f, 1.5f), absAngle(0), fov(0.66f) {
    Update();
}

Camera::Camera(Vector2 startPos, int startAngle, float f) {
    pos = startPos;
    absAngle = startAngle;
    fov = f;
    Update();
}

void Camera::Update(){
    dir.x = fcos(absAngle);
    dir.y = fsin(absAngle);
    plane.x = -dir.y * fov;
    plane.y =  dir.x * fov;
}

void Camera::Rotate(float angleDelta){
    absAngle += angleDelta;
    Update();
}

void Camera::Move(float distance, Map* map){
    Vector2 newPos = pos + (dir * distance);
    if(map->GetWorldTile(newPos.x, pos.y) == 0) pos.x = newPos.x;
    if(map->GetWorldTile(pos.x, newPos.y) == 0) pos.y = newPos.y;
}

void Camera::Strafe(float distance, Map* map){
    float newX = pos.x + (-dir.y * distance);
    float newY = pos.y + (dir.x * distance);
    
    if(map->GetWorldTile(newX, pos.y) == 0) pos.x = newX;
    if(map->GetWorldTile(pos.x, newY) == 0) pos.y = newY;
}
