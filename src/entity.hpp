#pragma once

#include "mathe.hpp"
#include "animation.hpp"
#include "chunk.hpp"

enum EntityFlags
{
    FLAG_NONE      = 0,
    FLAG_CHARACTER = 0x01,
    FLAG_PLAYER    = 0x02,
    FLAG_ENEMY     = 0x04,
    FLAG_LIFE      = 0x08
};

enum struct PlayerState
{
    idle,
    walk,
    run,
    swing,
    stab
};

enum struct EnemyState
{
    idle,
    chase
};

struct Entity
{
    V2 position = {0, 0};
    unsigned int flags;
    bool render = true;
    bool animation_enabled = true;
    Animation animation;
    Sprite sprite;
    Chunk* owner;
    struct 
    {
        int swings = 0;
        V2 last_direction;
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
        EnemyState state = EnemyState::idle;
    } enemy;
    struct
    {
        float hp = 100;
    } life;
};

constexpr float player_walk_speed = 100.0f;
constexpr float player_run_speed = 300.0f;

int find_free_entity();
Entity make_entity(V2 position, unsigned int flags);
void entity_update(Entity* entity);