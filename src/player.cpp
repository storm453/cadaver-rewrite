#include <stdio.h>
#include <stdlib.h>

#include "entity.h"
#include "game.h"
#include "mathe.h"
#include "animation.h"

Entity* make_player()
{
    Entity* entity = &game.entities[find_free_entity()];

    *entity = make_entity(entity_player, Vec2{0, 0});

    int entity_width, entity_height;
    
    entity->origin = { x: (float)entity_width / 2, y: (float)entity_height };

    return entity;
}

void step_player(Entity* entity)
{
    float speed = 255.0f;
    float acc = 10.0f;
        
    V2 target_velocity;

    if(game.window.input.shift)
    {
        speed = 500.0f;
    }

    target_velocity.x = (game.window.input.d - game.window.input.a) * speed;
    target_velocity.y = (game.window.input.s - game.window.input.w) * speed;
    
    entity->velocity.x += (target_velocity.x - entity->velocity.x) * acc * game.delta_time;
    entity->velocity.y += (target_velocity.y - entity->velocity.y) * acc * game.delta_time;

    entity->position.x += entity->velocity.x * game.delta_time;
    entity->position.y += entity->velocity.y * game.delta_time;
}