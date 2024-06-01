#include <cmath>
#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "entity.h"
#include "game.h"
#include "mathe.h"
#include "animation.h"

Entity* make_player()
{
    Entity* entity = &game.entities[find_free_entity()];

    *entity = make_entity(entity_player, Vec2{0, 0}, "player.png");

    int entity_width, entity_height;

    return entity;
}

void player_movement(Entity* entity, float speed)
{
    V2 target_velocity;

    target_velocity.x = (game.window.input.d - game.window.input.a) * speed;
    target_velocity.y = (game.window.input.s - game.window.input.w) * speed;
    
    //10 here is acceleration
    entity->velocity.x += (target_velocity.x - entity->velocity.x) * 10 * game.delta_time;
    entity->velocity.y += (target_velocity.y - entity->velocity.y) * 10 * game.delta_time;

    entity->position.x += entity->velocity.x * game.delta_time;
    entity->position.y += entity->velocity.y * game.delta_time;
}

void step_player(Entity* entity)
{
    switch(entity->player.state)
    {
        case(player_idle):
            player_movement(entity, player_walk_speed);

            if(sqrt(entity->velocity.x * entity->velocity.x + entity->velocity.y * entity->velocity.y) > player_walk_speed)
            {
                entity->player.state = player_move;
            }
        break; 

        case(player_move):
            player_movement(entity, 300.0f);
        break;
    }
}