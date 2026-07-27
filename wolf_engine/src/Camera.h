#ifndef CAMERA_H
#define CAMERA_H

#include "Mathutil.h"

class Camera{
public:
    Vector2 pos;
    Vector2 dir;
    Vector2 plane;
    Vector2 lastPos;
    
    float absAngle;
    float lastAngle;
    float fov;

    bool isDirty;

    Camera();
    Camera(Vector2 startPos, float startAngle, float fov = 0.66f);
    ~Camera() = default;

    void Update();

    void Rotate         (float angleDelta);
}; 

#endif
