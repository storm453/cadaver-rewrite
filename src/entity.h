#pragma once

#include "mathe.h"
#include "animation.h"

enum EntityType
{
    entity_none = 0,
    entity_object,
    entity_player,
    entity_enemy
};

struct Vec2
{
    float x;
    float y;
};

enum PlayerState
{
    player_idle,
    player_move
};

struct Entity
{
    EntityType type = entity_none;
    V2 position;
    V2 velocity;
    V2 origin;
    bool render = true;
    float depth;
    bool animation_enabled = false;
    Animation animation;
    struct 
    { 
        float hp;
        PlayerState state = player_idle;
    }player;
    Sprite sprite;
};

void entity_update(Entity* entity);
Entity make_entity(EntityType entityType, Vec2 entityPos, const char* filename);
int find_free_entity();