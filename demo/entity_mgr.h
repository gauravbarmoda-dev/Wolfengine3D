#ifndef ENTITY_MGR_H
#define ENTITY_MGR_H

#include "../engine/shlong.h"
#include <vector>

class Enemy;
class Player;

class Entity{
private:
    std::vector<Enemy*> enemies;

public:
    Entity() {}
    ~Entity();

    void AddEnemy(Enemy* enemy);

    void Update(Player* player, Camera* cam, Map* map, float dt);

    void QueueSprites(Rasterizer* rasterer);

    void Clear();
};

#endif
