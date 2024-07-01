#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "entity.hpp"
#include "main.hpp"

int find_free_entity()
{
    for(int i = 0; i < max_entity_count; i++)
    {
        if(game.entities[i].flags == FLAG_NONE)
        {
            return i;
        }
    }

    return -1;
}

Entity make_entity(V2 position, unsigned int flags)
{
    Entity temp = {};

    temp.position = position;
    temp.flags = flags;
    
    return temp;
}

void player_movement(Entity* entity, float speed)
{
    V2 input;

    input.x = (game.window.input.d - game.window.input.a);
    input.y = (game.window.input.s - game.window.input.w);

    if(input.x != 0)
    {
        entity->player.last_direction.x = input.x;
    }
    if(input.y != 0)
    {
        entity->player.last_direction.y = input.y;
    }

    input = normalize(input);

    entity->character.target_velocity.x = input.x * speed;
    entity->character.target_velocity.y = input.y * speed;
}

void player_attack(Entity* entity)
{
    if(game.window.input.mouse_down)
    {
        game.window.input.mouse_down = false;

        entity->animation.playback_time = 0;

        entity->player.swings++;

        if(entity->player.swings >= 3)
        {
            entity->player.state = PlayerState::stab;
            entity->player.swings = 0;
        }
        else
        {
            entity->player.state = PlayerState::swing;
        }
    }
}

void entity_update(Entity* entity)
{
    const int ACCEL = 100;

    //handle target velocity
    if(entity->flags & FLAG_CHARACTER)
    {
        entity->character.velocity.x += (entity->character.target_velocity.x - entity->character.velocity.x) * ACCEL * game.delta_time;
        entity->character.velocity.y += (entity->character.target_velocity.y - entity->character.velocity.y) * ACCEL * game.delta_time;

        entity->position = entity->position + entity->character.velocity * V2 {game.delta_time, game.delta_time};
    }

    if(entity->flags & FLAG_PLAYER)
    {
        switch(entity->player.state)
        {
            case(PlayerState::idle): {
                player_movement(entity, player_walk_speed);
                player_attack(entity);

                if(length(entity->character.velocity) > player_walk_speed / 2)
                {
                    entity->player.state = PlayerState::walk;
                }

                switch_animation(entity, entity->player.idle_animation);
            } break; 

            case(PlayerState::walk): {
                player_movement(entity, player_walk_speed);
                player_attack(entity);

                if(length(entity->character.velocity) < player_walk_speed / 2)
                {
                    entity->player.state = PlayerState::idle;
                }
                if(game.window.input.shift)
                {
                    entity->player.state = PlayerState::run;
                }

                switch_animation(entity, entity->player.walk_animation);
            } break;

            case(PlayerState::run): {
                player_movement(entity, player_run_speed);
                player_attack(entity);

                if(length(entity->character.velocity) < player_walk_speed / 2)
                {
                    entity->player.state = PlayerState::idle;
                }
                if(!game.window.input.shift)
                {
                    entity->player.state = PlayerState::walk;
                }

                switch_animation(entity, entity->player.run_animation);
            } break;

            case(PlayerState::swing): {
                player_movement(entity, player_walk_speed / 2);
                switch_animation(entity, entity->player.swing_animation);

                //check if the animation is done
                if(finished_animation(&entity->animation))
                {
                    entity->player.state = PlayerState::idle;
                }
            } break;
 
            case(PlayerState::stab): {
                int DASH_STRENGTH = 150;

                entity->character.target_velocity = {entity->player.last_direction.x * DASH_STRENGTH, entity->player.last_direction.y * DASH_STRENGTH};
                switch_animation(entity, entity->player.stab_animation);

                //check if the animation is done
                if(finished_animation(&entity->animation))
                {
                    entity->player.state = PlayerState::idle;
                }
            } break;
        }
    }

    if(entity->flags & FLAG_ENEMY)
    {
        float chase_speed = 80;

        if(game.player != NULL) 
        {
            V2 move = normalize(game.player->position - entity->position);

            entity->character.target_velocity = move * V2X(chase_speed);
        }
    }
}