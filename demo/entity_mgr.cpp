#include "entity_mgr.h"
#include "player.h"
#include "enemy.h"

Entity::~Entity() {
    Clear();
}

void Entity::Update(Player* player, Camera* cam, Map* map, float dt){
    for(Enemy* e : enemies){
        e->Update(this, player, cam, map, dt);
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
    
bool Entity::CheckWallCollision(float targetX, float targetY, float hitbox, Map* map){
    if (map->GetWorldTile(targetX + hitbox, targetY + hitbox) != 0) return true;
    if (map->GetWorldTile(targetX + hitbox, targetY - hitbox) != 0) return true;
    if (map->GetWorldTile(targetX - hitbox, targetY + hitbox) != 0) return true;
    if (map->GetWorldTile(targetX - hitbox, targetY - hitbox) != 0) return true;

    return false;
}

bool Entity::CheckEntityCollision(float targetX, float targetY, float hitbox, Enemy* self){
    for(Enemy* other : enemies){
        if(other == self) continue;
        if(other->GetCurrentState() == EnemyState::DEAD) continue;

        float diffX = targetX - other->GetSprite()->x;
        float diffY = targetY - other->GetSprite()->y;
        float distSqr = (diffX * diffX) + (diffY * diffY);

        float combinedHitbox = hitbox + other->GetHitbox();

        if(distSqr < (combinedHitbox * combinedHitbox)){
            return true;
        }
    }
    return false;
}

bool Entity::CheckPlayerCollision(float targetX, float targetY, float hitbox, Player* player){
    float diffX = targetX - player->GetSprite()->x;
    float diffY = targetY - player->GetSprite()->y;
    float distSqr = (diffX * diffX) + (diffY * diffY);

    float combinedHitbox = hitbox + player->GetHitbox();

    if(distSqr < (combinedHitbox * combinedHitbox)){
        return true;
    }
    return false;
}
    

void Entity::PlayerMeleeAttack(float px, float py, float dirX, float dirY, float range, float damage){
    float rangeSqr = range * range;
    
    for(Enemy* e : enemies){
        if(e->GetCurrentState() == EnemyState::DEAD) continue;
        
        float dx = e->GetSprite()->x - px;
        float dy = e->GetSprite()->y - py;
        float distSqr = dx * dx + dy * dy;

        if(distSqr <= rangeSqr){
            float dot = (dx * dirX) + (dy * dirY);
            if(dot > 0.0f){
                e->TakeDamage(damage);
            }
        }
    }
}


















