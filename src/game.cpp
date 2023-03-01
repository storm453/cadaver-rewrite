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
    {
        float speed = 0.15f;
        
        float playerX = entity->position.x;
        float playerY = entity->position.y;
        
        float moveX = (game.window.input.d - game.window.input.a);
        float moveY = (game.window.input.s - game.window.input.w);

        float moveLength = sqrtf(moveX * moveX + moveY * moveY);

        if(moveLength == 0) moveLength = 1;

        moveX /= moveLength;
        moveY /= moveLength;

        moveX *= speed;
        moveY *= speed;

        entity->position.x += moveX;
        entity->position.y += moveY;

        //printf("%f, %f\n", entity->position.x, entity->position.y);
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

Sprite MakeSprite(const char* path)
{
    Sprite temp;

    temp.sprite = IMG_Load(path);
    temp.texture = SDL_CreateTextureFromSurface(game.window.renderer, temp.sprite);

    return temp;
}

Entity MakeEntity(EntityType entityType, Vec2 entityPos, const char* spriteFile)
{
    Entity temp = {};

    temp.type = entityType;
    temp.position.x = entityPos.x;
    temp.position.y = entityPos.y;
    temp.sprite = MakeSprite(spriteFile);
    
    return temp;
}

int main()
{
    InitWindow(&game.window);

    for(int i = 0; i < 10; i++)
    {
        game.entities[FindFreeEntity()] = MakeEntity(entity_object, Vec2{ rand() / (float)RAND_MAX * 1240, rand() / (float)RAND_MAX * 700, }, "default.png");
    }

    //make the player entity
    game.entities[FindFreeEntity()] = MakeEntity(entity_player, Vec2{500, 500}, "ball.png");

    while(game.window.running)
    {
        UpdateWindow(&game.window);

        SDL_SetRenderDrawColor(game.window.renderer, game.window.input.mouseX / 5, game.window.input.mouseY / 5, 100, 255);
        SDL_RenderClear(game.window.renderer);

        for(int i = 0; i < max_entity_count; i++)
        {
            Entity* entity = &game.entities[i];

            EntityUpdate(entity);

            if(entity->type == entity_none) continue;

            SDL_Rect spriteRect
            {
                x: (int)entity->position.x,
                y: (int)entity->position.y,
                w: 35,
                h: 35
            };

            SDL_RenderCopy(game.window.renderer, entity->sprite.texture, NULL, &spriteRect);
        }

        SDL_RenderPresent(game.window.renderer);
    }
    
    CleanWindow(&game.window);
}