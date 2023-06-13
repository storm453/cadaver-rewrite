#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h> 

struct Sprite
{
    SDL_Surface* sprite;
    SDL_Texture* texture;
};

struct Animation
{
    Sprite frames[128];
    int frame_count;
    float frame_rate;
    float playback_time;
};

Sprite* step_animation(Animation* animation, float time_step);