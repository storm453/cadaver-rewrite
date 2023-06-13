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

void entity_update(Entity* entity);
Entity make_entity(EntityType entityType, Vec2 entityPos, const char* spriteFile);
int find_free_entity();

extern Game game;