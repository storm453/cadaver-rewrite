#pragma once
#include "window.h"

#define max_entity_count 999

struct Game
{
    Window window;
    Entity* player;
    Entity entities[max_entity_count];
    Entity* render_entities[max_entity_count];
    int render_amount;
    float delta_time;
};

Sprite make_sprite(const char* filename);

extern Game game;