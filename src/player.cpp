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

void move_player(Entity* entity, float amount)
{
    V2 target_velocity;

    target_velocity.x = 1 * amount;
    target_velocity.y = 1 * amount;
    
    entity->velocity.x += (target_velocity.x - entity->velocity.x) * 10 * game.delta_time;
    entity->velocity.y += (target_velocity.y - entity->velocity.y) * 10 * game.delta_time;

    entity->position.x += entity->velocity.x * game.delta_time;
    entity->position.y += entity->velocity.y * game.delta_time;
}

void player_attack(Entity* entity)
{
    if(game.window.input.mouse_down)
    {
        game.window.input.mouse_down = false;
        
        if(entity->player.combo >= 3)
        {
            entity->player.state = PlayerState::stab;
            entity->player.combo = 0;
        }
        else
        {
            entity->player.state = PlayerState::swing;
        }
        
        entity->player.combo++;
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

        case(PlayerState::swing):
            player_movement(entity, player_walk_speed / 2);

            if(entity->animation.dirty)
            {
                entity->animation.dirty = false;
                entity->player.state = PlayerState::idle;
            }

            switch_animation(entity, anim_player_swing);
        break;

        case(PlayerState::stab):
            move_player(entity, 300);

            if(entity->animation.dirty)
            {
                entity->animation.dirty = false;
                entity->player.state = PlayerState::idle;
            }

            switch_animation(entity, anim_player_attack);
        break;
    }
}