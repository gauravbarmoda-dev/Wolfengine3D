#ifndef PARTICLE_H
#define PARTICLE_H

#include "Raycaster.h"
#include <cstdint>

class Camera;
class Raycaster;
class Rasterizer;

class Particle{
private:
    static const int MAX_PARTICLES = 4096;

    float x[MAX_PARTICLES];
    float y[MAX_PARTICLES];
    float z[MAX_PARTICLES];

    float vx[MAX_PARTICLES];
    float vy[MAX_PARTICLES];
    float vz[MAX_PARTICLES];

    float life[MAX_PARTICLES];
    uint16_t color[MAX_PARTICLES];

    int activeCount;

public:
    Particle();
    ~Particle();

    void Emit(float px, float py, float pz, float vx, float vy, float vz, float pLife, uint16_t color);

    void Update(float dt);

    void Render(Rasterizer* raster, Camera* cam, Raycaster* ray);
};

#endif
