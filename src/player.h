#pragma once

#include "entity.h"

constexpr float player_walk_speed = 100.0f;
constexpr float player_run_speed = 300.0f;

Entity* make_player();
void player_movement(Entity* entity, float speed);
void step_player(Entity* entity);