#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "entity.h"
#include "game.h"

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

Entity make_entity(EntityType entityType, Vec2 entityPos, const char* filename)
{
    Entity temp = {};

    temp.velocity.x = 0;
    temp.velocity.y = 0;

    temp.origin.x = 0;
    temp.origin.y = 0;

    temp.type = entityType;
    temp.position.x = entityPos.x;
    temp.position.y = entityPos.y;

    temp.sprite = make_sprite(filename);
    
    return temp;
}

void player_movement(Entity* entity, float speed)
{
    V2 target_velocity;

    target_velocity.x = (game.window.input.d - game.window.input.a) * speed;
    target_velocity.y = (game.window.input.s - game.window.input.w) * speed;
    
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

void entity_update(Entity* entity)
{
    entity->depth = -(entity->position.y);
    
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

                    switch_animation(entity, anim_player_idle);
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

                    switch_animation(entity, anim_player_walk);
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

                    switch_animation(entity, anim_player_run);
                } break;

                case(PlayerState::swing): {
                    

                    if(entity->animation.dirty)
                    {
                        entity->animation.dirty = false;
                        entity->player.state = PlayerState::idle;
                    }

                    switch_animation(entity, anim_player_swing);
                } break;

                case(PlayerState::stab): {
                    player_movement(entity, 300);

                    if(entity->animation.dirty)
                    {
                        entity->animation.dirty = false;
                        entity->player.state = PlayerState::idle;
                    }

                    switch_animation(entity, anim_player_attack);
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

            diff.x = game.player->position.x - entity->position.x;
            diff.y = game.player->position.y - entity->position.y;

            float norm = length(diff);

            if(norm != 0)
            {
                entity->position.x += (diff.x / norm) * chase_speed;
                entity->position.y += (diff.y / norm) * chase_speed;
            }
        }
        break;
    }
}