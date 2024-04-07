#include "window.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>

void init_window(Window* window)
{
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

    Uint32 flags = SDL_WINDOW_OPENGL;

    window->window = SDL_CreateWindow("Cadaver 2!!!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window->width, window->height, flags);
    window->running = true;
    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED);
    window->context = SDL_GL_CreateContext(window->window);
}

void update_window(Window* window)
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
            if(ev.key.keysym.sym == SDLK_LSHIFT) window->input.shift = true;

            if(ev.key.keysym.sym == SDLK_ESCAPE) window->running = false;
        }
        if(ev.type == SDL_KEYUP)
        {
            if(ev.key.keysym.sym == SDLK_w) window->input.w = false;
            if(ev.key.keysym.sym == SDLK_a) window->input.a = false;
            if(ev.key.keysym.sym == SDLK_s) window->input.s = false;
            if(ev.key.keysym.sym == SDLK_d) window->input.d = false;
            if(ev.key.keysym.sym == SDLK_LSHIFT) window->input.shift = false;
        }
        if(ev.type == SDL_MOUSEMOTION)
        {
            window->input.mouseX = ev.motion.x;
            window->input.mouseY = ev.motion.y;
        }
    }
}

void clean_window(Window* window)
{
    SDL_DestroyWindow(window->window);
    SDL_DestroyRenderer(window->renderer);
}