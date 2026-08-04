#include "Camera.h"

Camera::Camera() : pos(1.5f, 1.5f), absAngle(0.0f), fov(0.66f){
    z = 0.0f;
    pitch = 0;
    Update();
}

Camera::Camera(Vector2 startPos, float startAngle, float f){
    pos = startPos;
    absAngle = startAngle;
    fov = f;
    z = 0.0f;
    pitch = 0;
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
