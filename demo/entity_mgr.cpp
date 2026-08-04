#include "entity_mgr.h"
#include "enemy.h"

Entity::~Entity() {
    Clear();
}

void Entity::Update(Player* player, Camera* cam, Map* map, float dt){
    for(Enemy* e : enemies){
        e->Update(player, cam, map, dt);
    }
}

void Entity::QueueSprites(Rasterizer* rasterizer){
    for(Enemy* e : enemies){
        rasterizer->QueueSprite(e->GetSprite());
    }
}

void Entity::AddEnemy(Enemy* enemy){
    if(enemy){
        enemies.push_back(enemy);
    }
}

void Entity::Clear(){
    for(Enemy* e : enemies){
        delete e;
    }
    enemies.clear();
}
