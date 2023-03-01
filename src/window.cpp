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
            if(ev.key.keysym.sym == SDLK_w) window->input.w = true;
            if(ev.key.keysym.sym == SDLK_a) window->input.a = true;
            if(ev.key.keysym.sym == SDLK_s) window->input.s = true;
            if(ev.key.keysym.sym == SDLK_d) window->input.d = true;

            if(ev.key.keysym.sym == SDLK_ESCAPE) window->running = false;
        }
        if(ev.type == SDL_KEYUP)
        {
            if(ev.key.keysym.sym == SDLK_w) window->input.w = false;
            if(ev.key.keysym.sym == SDLK_a) window->input.a = false;
            if(ev.key.keysym.sym == SDLK_s) window->input.s = false;
            if(ev.key.keysym.sym == SDLK_d) window->input.d = false;
        }
        if(ev.type == SDL_MOUSEMOTION)
        {
            window->input.mouseX = ev.motion.x;
            window->input.mouseY = ev.motion.y;
        }
    }
}

void CleanWindow(Window* window)
{
    SDL_DestroyWindow(window->window);
    SDL_DestroyRenderer(window->renderer);
}