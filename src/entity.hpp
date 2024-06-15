#pragma once

#include "mathe.hpp"
#include "animation.hpp"

enum struct EntityType
{
    NONE = 0x01,
    OBJECT = 0x02,
    PLAYER = 0x04,
    ENEMY = 0x08
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
    EntityType type = EntityType::NONE;
    unsigned int flags;
    V2 position = {0, 0};
    bool render = true;
    bool animation_enabled = false;
    Animation animation;
    struct 
    {
        float hp;
        int swings = 0;
        PlayerState state = PlayerState::idle;
        Animation* idle_animation;
        Animation* walk_animation;
        Animation* run_animation;
        Animation* swing_animation;
        Animation* stab_animation;
    }player;
    struct
    {
        V2 velocity = {0,0 };
        V2 target_velocity = {0, 0};
    } character;
    struct
    {
        PlayerState state;
    } enemy;
};

constexpr float player_walk_speed = 100.0f;
constexpr float player_run_speed = 300.0f;

int find_free_entity();
Entity make_entity(EntityType entityType, V2 entityPos);
void entity_update(Entity* entity);