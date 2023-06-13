#include "animation.h"

Sprite* step_animation(Animation* animation, float time_step)
{
    animation->playback_time += time_step;

    int current_frame = (int)(animation->playback_time / animation->frame_rate);

    if(current_frame >= animation->frame_count)
    {
        animation->playback_time = 0;

        current_frame = 0;
    }

    return &animation->frames[current_frame];
}