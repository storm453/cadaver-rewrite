#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h> 
#include "mathe.h"
#include "animation.h"

enum EntityType
{
    entity_none = 0,
    entity_object,
    entity_player
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
    V2 position = { x: 0, y: 0 };
    V2 velocity = { x: 0, y: 0 };
    V2 origin = { x: 0, y: 0 };
    Sprite sprite;
    bool render = true;
    float depth;
    bool animation_enabled = false;
    Animation animation;
    struct 
    { 
        float hp;
        PlayerState state;
    }player;
};

void entity_update(Entity* entity);
Entity make_entity(EntityType entityType, Vec2 entityPos, const char* spriteFile);
int find_free_entity();