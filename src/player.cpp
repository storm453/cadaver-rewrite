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

void player_attack(Entity* entity)
{
    if(game.window.input.mouse_down)
    {
        entity->player.state = PlayerState::attack;
        entity->animation.playback_time = 0;
    }
}

void step_player(Entity* entity)
{
    switch(entity->player.state)
    {
        case(PlayerState::idle):
            player_movement(entity, player_walk_speed);
            player_attack(entity);

            if(length(entity->velocity) > player_walk_speed / 2)
            {
                entity->player.state = PlayerState::walk;
            }

            switch_animation(entity, anim_player_idle);
        break; 

        case(PlayerState::walk):
            player_movement(entity, player_walk_speed);
            player_attack(entity);

            if(length(entity->velocity) < player_walk_speed / 2)
            {
                entity->player.state = PlayerState::idle;
            }
            if(game.window.input.shift)
            {
                entity->player.state = PlayerState::run;
            }

            switch_animation(entity, anim_player_walk);
        break;

        case(PlayerState::run):
            player_movement(entity, player_run_speed);
            player_attack(entity);

            if(length(entity->velocity) < player_walk_speed / 2)
            {
                entity->player.state = PlayerState::idle;
            }
            if(!game.window.input.shift)
            {
                entity->player.state = PlayerState::walk;
            }

            switch_animation(entity, anim_player_run);
        break;

        case(PlayerState::attack):
            if(entity->animation.dirty)
            {
                entity->animation.dirty = false;
                entity->player.state = PlayerState::idle;
            }

            switch_animation(entity, anim_player_attack);
        break;
    }
}