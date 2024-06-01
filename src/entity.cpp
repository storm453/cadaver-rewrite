#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "entity.h"
#include "game.h"
#include "player.h"

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

void entity_update(Entity* entity)
{
    entity->depth = -(entity->position.y);
    
    switch(entity->type)
    {
        case(entity_player):
        {
            step_player(entity);
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

            float diff_x = game.player->position.x - entity->position.x;
            float diff_y = game.player->position.y - entity->position.y;

            float norm = sqrt((diff_x * diff_x) + (diff_y * diff_y));

            if(norm != 0)
            {
                entity->position.x += (diff_x / norm) * chase_speed;
                entity->position.y += (diff_y / norm) * chase_speed;
            }
        }
        break;
    }
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