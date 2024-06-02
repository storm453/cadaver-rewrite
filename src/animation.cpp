#include "animation.h"
#include "entity.h"
#include <stdio.h>

Sprite* step_animation(Animation* animation, float time_step)
{
    animation->playback_time += time_step;

    int current_frame = (int)(animation->playback_time / animation->frame_rate);
    
    if(current_frame >= animation->frame_count)
    {
        animation->dirty = true;
            
        animation->playback_time = 0;

        current_frame = 0;
    }

    return &animation->frames[current_frame];
}

void switch_animation(Entity* entity, Animation new_animation)
{
    float old_playback_time = entity->animation.playback_time;

    entity->animation = new_animation;
    entity->animation.playback_time = old_playback_time;
}