#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>

#define max_entity_count 999

//additional utils
#include "window.h"
#include "entity.h"
#include "game.h"

Game game;

int FindFreeEntity()
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

void EntityUpdate(Entity* entity)
{
   switch(entity->type)
   {
    case(entity_player):
        entity->position.x = game.window.mouseX;
        entity->position.y = game.window.mouseY;
    break;

    case(entity_none):
    
    break;

    case(entity_object):

    break;
   }
}
        
Entity MakeEntity(EntityType entityType, Vec2 entityPos)
{
    Entity temp;

    temp.type = entityType;
    temp.position.x = entityPos.x;
    temp.position.y = entityPos.y;

    return temp;
}

int main()
{
    InitWindow(&game.window);

    for(int i = 0; i < 10; i++)
    {
        game.entities[FindFreeEntity()] = MakeEntity(entity_object, Vec2{ rand() / (float)RAND_MAX * 500, rand() / (float)RAND_MAX * 500, });
    }

    //make the player entity
    game.entities[FindFreeEntity()] = MakeEntity(entity_player, Vec2{500, 500});

    while(game.window.running)
    {
        UpdateWindow(&game.window);

        SDL_SetRenderDrawColor(game.window.renderer, game.window.mouseX / 5, game.window.mouseY / 5, 100, 255);
        SDL_RenderClear(game.window.renderer);

        SDL_Surface* sprite = IMG_Load("square.png");
        SDL_Texture* texture = SDL_CreateTextureFromSurface(game.window.renderer, sprite);

        for(int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &game.entities[i];

            EntityUpdate(entity);

            if(entity->type == entity_none) continue;

            SDL_Rect spriteRect
            {
                x: (int)entity->position.x,
                y: (int)entity->position.y,
                w: 25,
                h: 25
            };

            SDL_RenderCopy(game.window.renderer, texture, NULL, &spriteRect);
        }

        SDL_RenderPresent(game.window.renderer);
    }
    
    CleanWindow(&game.window);
}