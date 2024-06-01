#include <stdio.h>
#include <stdlib.h>

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
            // float diff_x = game.player->position.x - entity->position.x;
            // float diff_y = game.player->position.y - entity->position.y;

            // float norm = sqrt(diff_x * diff_x + diff_y * diff_y);

            // entity->position.x += (diff_x / norm);
            // entity->position.y += (diff_y / norm);
        }
        break;
    }
}

Entity make_entity(EntityType entityType, Vec2 entityPos, const char* filename)
{
    Entity temp = {};

    temp.type = entityType;
    temp.position.x = entityPos.x;
    temp.position.y = entityPos.y;

    temp.sprite = make_sprite(filename);
    
    return temp;
}