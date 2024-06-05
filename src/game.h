#pragma once
#include "window.h"
#include "entity.h"
#include <glm/glm.hpp>

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

struct Camera
{
    glm::vec2 pos = { 0, 0 };
    glm::mat4 projection = glm::mat4(1.0f);
    float zoom = 0.5;
};

extern Game game;
