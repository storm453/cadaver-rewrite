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

struct Entity
{
    EntityType type = entity_none;
    V2 position = { x: 0, y: 0 };
    V2 velocity = { x: 0, y: 0 };
    V2 origin = { x: 0, y: 0 };
    Sprite sprite;
    bool render = true;
    float depth;
    Animation animation;
};