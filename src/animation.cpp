#include <stdio.h>

#include "animation.hpp"
#include "entity.hpp"
#include "main.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include "stb_image.h"

Sprite make_sprite(const char* filename)
{
    Sprite temp;
    
    glGenTextures(1, &temp.texture);
    glBindTexture(GL_TEXTURE_2D, temp.texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

    temp.width = width;
    temp.height = height;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return temp;
}

Sprite* step_animation(Animation* animation, float time_step)
{
    animation->playback_time += time_step;

    int current_frame = (int)(animation->playback_time / animation->frame_rate);
    
    if(current_frame == animation->frame_count)
    {
        if(animation->playback_time != 0)
        {
            //animation->dirty = true;
            animation->playback_time = 0;

            current_frame = 0;
        }
    }

    return &animation->frames[current_frame];
}

bool finished_animation(Animation* animation)
{
    if(animation->frame_rate != 0)
    {
        int current_frame = (int)((animation->playback_time + game.delta_time) / animation->frame_rate);
        
        if(current_frame == animation->frame_count)
        {
            return true;
            exit;
        }
    }

    return false;
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
    std::string data[4];

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

        temp.frames[i] = make_sprite(frame.c_str());
        temp.frame_count = loop;
        temp.frame_rate = stof(data[3]);
    }

    return temp;
}