#include "window.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>

void InitWindow(Window* window)
{
    window->window = SDL_CreateWindow("Tynebourne", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    window->running = true;
    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
}

void UpdateWindow(Window* window)
{
    SDL_Event ev;
    
    while(SDL_PollEvent(&ev))
    {
        if(ev.type == SDL_QUIT)
        {
            window->running = false;
        }
        if(ev.type == SDL_KEYDOWN)
        {
            if(ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                window->running = false;
            }
        }
        if(ev.type == SDL_MOUSEMOTION)
        {
            window->mouseX = ev.motion.x;
            window->mouseY = ev.motion.y;
        }
    }
}

void CleanWindow(Window* window)
{
    SDL_DestroyWindow(window->window);
    SDL_DestroyRenderer(window->renderer);
}