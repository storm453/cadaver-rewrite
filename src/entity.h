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

enum struct PlayerState
{
    idle,
    walk,
    run,
    swing,
    stab
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
        int combo = 0;
        PlayerState state = PlayerState::idle;
    }player;
    Sprite sprite;
};

constexpr float player_walk_speed = 100.0f;
constexpr float player_run_speed = 300.0f;

int find_free_entity();
Entity make_entity(EntityType entityType, Vec2 entityPos, const char* filename);
void entity_update(Entity* entity);