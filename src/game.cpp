#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

#include <cstdint>

//additional utils
#include "window.h"
#include "entity.h"
#include "game.h"
#include "player.h"
#include "animation.h"
#include "chunk.h"

#include "SimplexNoise.h"

Game game;

int find_free_entity()
{
    for(int i = 0; i < max_entity_count; i++)
    {
        if(game.entities[i].type == entity_none)
        {
            return i;
        }
    }

    return -1;
}

float lerp(float a, float b, float t)
{
    return a * (1 - t) + b * t;
}

void entity_update(Entity* entity)
{
    // int entity_height;

    // SDL_QueryTexture(entity->sprite.texture, NULL, NULL, NULL, &entity_height);
    
    entity->depth = -(entity->position.y);
    
    switch(entity->type)
    {
        case(entity_player):
        {
            step_player(entity);
        }
        break;

        case(entity_none):
        {
            
        }
        break;

        case(entity_object):
        {
            
        }   
        break;
    }
}

Sprite make_sprite(const char* path)
{
    Sprite temp;

    temp.sprite = IMG_Load(path);
    temp.texture = SDL_CreateTextureFromSurface(game.window.renderer, temp.sprite);
    
    return temp;
}

Entity make_entity(EntityType entityType, Vec2 entityPos, const char* spriteFile)
{
    Entity temp = {};

    temp.type = entityType;
    temp.position.x = entityPos.x;
    temp.position.y = entityPos.y;
    temp.sprite = make_sprite(spriteFile);
    
    return temp;
}

Entity find_player_entity()
{
    Entity temp;

    for(int i = 0; i < max_entity_count; i++)
    {
        if(game.entities[i].type == entity_player)
        {
            temp = game.entities[i];
        }
    }
    
    return temp;
}

Camera camera;

Chunk chunks_array[999];

int main()
{
    init_window(&game.window);

    Animation player_idle = {};

    player_idle.frames[0] = make_sprite("playeridle1.png");
    player_idle.frames[1] = make_sprite("playeridle2.png");
    player_idle.frames[2] = make_sprite("playeridle3.png");
    player_idle.frames[3] = make_sprite("playeridle4.png");
    player_idle.frames[4] = make_sprite("playeridle5.png");
    player_idle.frames[5] = make_sprite("playeridle6.png");
    player_idle.frames[6] = make_sprite("playeridle7.png");
    player_idle.frames[7] = make_sprite("playeridle8.png");
    player_idle.frames[8] = make_sprite("playeridle9.png");
    player_idle.frames[9] = make_sprite("playeridle10.png");
    
    player_idle.frame_count = 10;
    player_idle.frame_rate = 0.1;

    for(int i = 0; i < 3; i++)
    {
        Entity* entity = &game.entities[find_free_entity()];

        *entity = make_entity(entity_object, Vec2{ rand() / (float)RAND_MAX * 1240, rand() / (float)RAND_MAX * 700, }, "assets/dev/tree.png");

        int entity_width, entity_height;

        SDL_QueryTexture(entity->sprite.texture, NULL, NULL, &entity_width, &entity_height);
        
        entity->origin = { x: (float)entity_width / 2, y: (float)entity_height };

        entity->animation = player_idle;
        entity->animation_enabled = true;
    }

    //make the player entity
    {
        game.player = make_player();
    }

    float last_time = SDL_GetTicks();

    while(game.window.running)
    {
        V2i chunk_index = get_chunk_index(game.player->position.x, game.player->position.y);

        for(int i = -chunk_load; i <= chunk_load; i++)
        {
            for(int j = -chunk_load; j <= chunk_load; j++)
            {
                V2i loop_chunk_index = { chunk_index.x + i, chunk_index.y + j };

                Chunk* check_chunk = lookup_chunk(loop_chunk_index, array_size(chunks_array), chunks_array);

                if(check_chunk == NULL)
                {
                    //chunk doesn't exist, make it
                    Chunk* new_chunk = find_free_chunk_slot(array_size(chunks_array), chunks_array);
                    
                    if(new_chunk != NULL)
                    {
                        new_chunk->index = loop_chunk_index;
                        new_chunk->exists = true;
                    }
                }
                else
                {
                    //chunk does exist, so move it to the top of the array
                }
            }
        }

        unsigned int start_time = SDL_GetTicks();

        update_window(&game.window);

        SDL_SetRenderDrawColor(game.window.renderer, 69, 155, 0, 255);
        SDL_RenderClear(game.window.renderer);

        float target_x = game.player->position.x - game.window.width / 2 + game.player->origin.x / 2;
        float target_y = game.player->position.y - game.window.height / 2;

        camera.x = lerp(camera.x, target_x, 0.1);
        camera.y = lerp(camera.y, target_y, 0.1);

        game.render_amount = 0;

        //render chunks
        for(int i = 0; i < array_size(chunks_array); i++)
        {
            Chunk* current_chunk = &chunks_array[i];

            V2i chunk_physical = { current_chunk->index.x * chunk_size, current_chunk->index.y * chunk_size };

            SDL_FRect chunk_rect
            {
                x: chunk_physical.x - camera.x,
                y: chunk_physical.y - camera.y,
                w: (float) chunk_size,
                h: (float) chunk_size
            };

            float noise = SimplexNoise::noise(current_chunk->index.x * 2, current_chunk->index.y * 2);
            (void)noise;

            //srand(i);

            noise = (noise + 1) / 2;

            SDL_SetRenderDrawColor(game.window.renderer, 255 * noise, 0, 0, 255);
            SDL_RenderFillRectF(game.window.renderer, &chunk_rect);
        }

        for(int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &game.entities[i];

            if(entity->type == entity_none) continue;

            entity_update(entity);
                
            if(entity->render)
            {
                game.render_entities[game.render_amount] = entity;

                ++game.render_amount;
            }
        }
        for(int i = 0; i < game.render_amount; i++)
        {
            int closest = i;

            for(int j = i + 1; j < game.render_amount; j++)
            {
                if(game.render_entities[closest]->depth < game.render_entities[j]->depth)
                {
                    closest = j;
                }
            }

            Entity* oldEntity = game.render_entities[closest];

            game.render_entities[closest] = game.render_entities[i];
            game.render_entities[i] = oldEntity;
        }
        for(int j = 0; j < game.render_amount; j++)
        {
            Entity* render_entity = game.render_entities[j]; 

            int sprite_width, sprite_height;
            
            SDL_Texture* current_texture;

            if(render_entity->animation_enabled)
            {
                current_texture = step_animation(&render_entity->animation, game.delta_time)->texture;
            }
            else
            {
                current_texture = render_entity->sprite.texture;
            }

            SDL_QueryTexture(current_texture, NULL, NULL, &sprite_width, &sprite_height);

            SDL_Rect sprite_rect
            {
                x: (int)(render_entity->position.x - camera.x - render_entity->origin.x),
                y: (int)(render_entity->position.y - camera.y - render_entity->origin.y),
                w: sprite_width,
                h: sprite_height
            };

            SDL_RenderCopy(game.window.renderer, current_texture, NULL, &sprite_rect);
        }

        //draw text
        TTF_Init();

        TTF_Font *font = TTF_OpenFont("Roboto-Regular.ttf", 48);

        char fps_display[100];
        
        float time = SDL_GetTicks();
        float dt = time - last_time;

        last_time = time;

        float fps = 1000.0f / dt;
        
        //snprintf(fps_display, "%f", fps);
        
        SDL_Rect text_rect;
        SDL_Surface* text_surf = TTF_RenderText_Blended(font, fps_display, { 255, 255, 255 });
        SDL_Texture *text;
        
        text = SDL_CreateTextureFromSurface(game.window.renderer, text_surf);

        text_rect.x = 20;
		text_rect.y = 200;
		text_rect.w = text_surf->w;
		text_rect.h = text_surf->h;

        SDL_RenderCopy(game.window.renderer, text, NULL, &text_rect);

        TTF_Quit();
        
        //render polygon
        

        //SDL_RenderPresent(game.window.renderer); //NOT OPENGL
        SDL_GL_SwapWindow(game.window.window);
    
        unsigned int end_time = SDL_GetTicks();

        game.delta_time = (end_time - start_time) / 1000.f;
    }
    
    clean_window(&game.window);
}