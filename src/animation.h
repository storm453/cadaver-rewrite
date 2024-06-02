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
    bool dirty;
};

Sprite make_sprite(const char* filename);
Sprite* step_animation(Animation* animation, float time_step);
void switch_animation(Entity* entity, Animation new_animation);
Animation load_animation(char* filename);