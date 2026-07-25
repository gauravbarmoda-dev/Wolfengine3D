#ifndef CAMERA_H
#define CAMERA_H

#include "mathutil.h"

class Map;
class Camera{
public:
    Vector2 pos;
    Vector2 dir;
    Vector2 plane;

    float absAngle;
    float fov;

    Camera();
    Camera(Vector2 startPos, float startAngle, float fov = 0.66f);
    ~Camera() = default;

    void Update();

    void Rotate         (float angleDelta);
    void Move           (float distance, Map* map);
    void Strafe         (float distance, Map* map);
}; 

#endif
