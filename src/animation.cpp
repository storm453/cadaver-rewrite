#include <stdio.h>

#include "animation.hpp"
#include "entity.hpp"
#include "game.hpp"

#include <iostream>
#include <fstream>
#include <string>

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

void switch_animation(Entity* entity, Animation* new_animation)
{
    float old_playback_time = entity->animation.playback_time;

    entity->animation = *new_animation;
    entity->animation.playback_time = old_playback_time;
}

std::string char_to_string(const char* data)
{
    std::string string;

    int size = strlen(data);

    string.assign(data, size);

    return string;
}

Animation make_animation_txt(const char* filename)
{
    std::string string_name = char_to_string(filename);

    std::ifstream file(string_name);

    std::string line;
    std::string data[3];

    int iterator = 0;

    if(file.is_open())
    {
        while(std::getline(file, line))
        {
            data[iterator] = line;

            iterator++;
        }

        file.close();
    }

    int loop = stoi(data[2]);

    Animation temp;

    for(int i = 0; i < loop; i++)
    {
        std::string frame = data[0] + std::to_string(i) + data[1];

        std::cout << frame << "\n";

        //temp.frames[0] = make_sprite("assets/player/playeridle1.png");
    }

    return temp;
}