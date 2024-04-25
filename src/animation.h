#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h> 

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