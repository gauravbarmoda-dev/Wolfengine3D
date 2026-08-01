#ifndef ENTITY_MGR_H
#define ENTITY_MGR_H

#include "../engine/shlong.h"
#include <vector>

class Enemy;

class Entity{
private:
    std::vector<Enemy*> enemies;

public:
    Entity() {}
    ~Entity();

    void AddEnemy(Enemy* enemy);

    void Update(Camera* cam, float dt);

    void QueueSprites(Rasterizer* rasterer);

    void Clear();
};

#endif
