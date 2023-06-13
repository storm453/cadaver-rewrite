#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>

//additional utils
#include "window.h"
#include "entity.h"
#include "game.h"
#include "player.h"
#include "animation.h"

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

int main()
{
    init_window(&game.window);

    for(int i = 0; i < 3; i++)
    {
        Entity* entity = &game.entities[find_free_entity()];

        *entity = make_entity(entity_object, Vec2{ rand() / (float)RAND_MAX * 1240, rand() / (float)RAND_MAX * 700, }, "tree.png");

        int entity_width, entity_height;

        SDL_QueryTexture(entity->sprite.texture, NULL, NULL, &entity_width, &entity_height);
        
        entity->origin = { x: (float)entity_width / 2, y: (float)entity_height };

        // entity->animation.frames[0] = make_sprite("ball_run1.png");
        // entity->animation.frames[1] = make_sprite("ball_run2.png");
        // entity->animation.frames[2] = make_sprite("ball_run3.png");
        // entity->animation.frames[3] = make_sprite("ball_run4.png");
        
        // entity->animation.frame_count = 4;
        // entity->animation.frame_rate = 0.1;
    }

    //make the player entity
    {
        game.player = make_player();
    }

    while(game.window.running)
    {
        unsigned int start_time = SDL_GetTicks();

        update_window(&game.window);

        SDL_SetRenderDrawColor(game.window.renderer, 194, 214, 79, 255);
        SDL_RenderClear(game.window.renderer);

        float target_x = game.player->position.x - game.window.width / 2 + game.player->origin.x / 2;
        float target_y = game.player->position.y - game.window.height / 2;

        camera.x = lerp(camera.x, target_x, 0.001);
        camera.y = lerp(camera.y, target_y, 0.001);

        game.render_amount = 0;

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
            
            SDL_QueryTexture(render_entity->sprite.texture, NULL, NULL, &sprite_width, &sprite_height);

            SDL_Rect sprite_rect
            {
                x: (int)(render_entity->position.x - camera.x - render_entity->origin.x),
                y: (int)(render_entity->position.y - camera.y - render_entity->origin.y),
                w: sprite_width,
                h: sprite_height
            };

            SDL_Rect origin_rect
            {
                x: (int)(render_entity->position.x - 3 - camera.x),
                y: (int)(render_entity->position.y - 3 - camera.y),
                w: 6,
                h: 6
                
            };

            if(render_entity->animation.frame_count > 0)
            {
                SDL_RenderCopy(game.window.renderer, step_animation(&render_entity->animation, game.delta_time)->texture, NULL, &sprite_rect);
            }
            else
            {
                SDL_RenderCopy(game.window.renderer, render_entity->sprite.texture, NULL, &sprite_rect);
            }
            

            SDL_SetRenderDrawColor(game.window.renderer, 100, 190, 255, 255);
            SDL_RenderFillRect(game.window.renderer, &origin_rect);
        }

        SDL_RenderPresent(game.window.renderer);
    
        unsigned int end_time = SDL_GetTicks();

        game.delta_time = (end_time - start_time) / 1000.f;
    }
    
    clean_window(&game.window);
}