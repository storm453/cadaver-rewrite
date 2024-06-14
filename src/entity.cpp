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
        if(game.entities[i].type == entity_none)
        {
            return i;
        }
    }

    return -1;
}

Entity make_entity(EntityType entityType, V2 entityPos, const char* filename)
{
    Entity temp = {};

    temp.velocity.x = 0;
    temp.velocity.y = 0;

    temp.type = entityType;
    temp.position.x = entityPos.x;
    temp.position.y = entityPos.y;
    
    return temp;
}

void player_movement(Entity* entity, float speed)
{
    V2 target_velocity;

    target_velocity.x = (game.window.input.d - game.window.input.a) * speed;
    target_velocity.y = (game.window.input.s - game.window.input.w) * speed;
    
    entity->velocity.x += (target_velocity.x - entity->velocity.x) * 500 * game.delta_time;
    entity->velocity.y += (target_velocity.y - entity->velocity.y) * 500 * game.delta_time;

    entity->position.x += target_velocity.x * 10 * game.delta_time; //entity->velocity.x * game.delta_time;
    entity->position.y += target_velocity.y * 10  * game.delta_time; //entity->velocity.y * game.delta_time;
}

void player_attack(Entity* entity)
{
    if(game.window.input.mouse_down)
    {
        game.window.input.mouse_down = false;

        entity->animation.playback_time = 0;
        entity->player.state = PlayerState::swing;
    }
}

void entity_update(Entity* entity)
{
    switch(entity->type)
    {
        case(entity_player):
        {
            switch(entity->player.state)
            {
                case(PlayerState::idle): {
                    player_movement(entity, player_walk_speed);
                    player_attack(entity);

                    if(length(entity->velocity) > player_walk_speed / 2)
                    {
                        entity->player.state = PlayerState::walk;
                    }

                    switch_animation(entity, entity->player.idle_animation);
                } break; 

                case(PlayerState::walk): {
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

                    switch_animation(entity, entity->player.walk_animation);
                } break;

                case(PlayerState::run): {
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

                    switch_animation(entity, entity->player.run_animation);
                } break;

                case(PlayerState::swing): {
                    

                    if(entity->animation.dirty)
                    {
                        entity->animation.dirty = false;
                        entity->player.state = PlayerState::idle;
                    }

                    switch_animation(entity, entity->player.swing_animation);
                } break;

                case(PlayerState::stab): {
                    player_movement(entity, 300);

                    if(entity->animation.dirty)
                    {
                        // entity->animation.dirty = false;
                        // entity->player.state = PlayerState::idle;
                    }

                    switch_animation(entity, entity->player.stab_animation);
                } break;
            }
        }
        break;

        case(entity_none):
        {
            
        }
        break;

        case(entity_object):
        {
            
        }   
        break;

        case(entity_enemy):
        {
            float chase_speed = 0.1;

            V2 diff;


            if(game.player != NULL) 
            {
                diff.x = game.player->position.x - entity->position.x;
                diff.y = game.player->position.y - entity->position.y;

                float norm = length(diff);

                if(norm != 0)
                {
                    entity->position.x += (diff.x / norm) * chase_speed;
                    entity->position.y += (diff.y / norm) * chase_speed;
                }
            }
        }
        break;
    }
}