#pragma once

struct Entity;

struct Sprite
{
    unsigned int texture;
    int width, height;
};

struct Animation
{
    Sprite frames[128];
    int frame_count;
    float frame_rate;
    float playback_time;
};

Sprite make_sprite(const char* filename);
Sprite* step_animation(Animation* animation, float time_step);
bool finished_animation(Animation* animation);
void switch_animation(Entity* entity, Animation* new_animation);