#include "window.hpp"
#include <SDL2/SDL.h>

void init_window(Window* window)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    Uint32 flags = SDL_WINDOW_OPENGL;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window->window = SDL_CreateWindow("Cadaver 2!!!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window->width, window->height, flags);
    window->context = SDL_GL_CreateContext(window->window);
    window->running = true;
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
            if(ev.key.keysym.sym == SDLK_l) window->input.l = true;
            if(ev.key.keysym.sym == SDLK_LSHIFT) window->input.shift = true;

            if(ev.key.keysym.sym == SDLK_ESCAPE) window->running = false;
        }
        if(ev.type == SDL_KEYUP)
        {
            if(ev.key.keysym.sym == SDLK_w) window->input.w = false;
            if(ev.key.keysym.sym == SDLK_a) window->input.a = false;
            if(ev.key.keysym.sym == SDLK_s) window->input.s = false;
            if(ev.key.keysym.sym == SDLK_d) window->input.d = false;
            if(ev.key.keysym.sym == SDLK_l) window->input.l = false;
            if(ev.key.keysym.sym == SDLK_LSHIFT) window->input.shift = false;
        }
        if(ev.type == SDL_MOUSEMOTION)
        {
            SDL_GetMouseState(&window->input.mouseX, &window->input.mouseY);
        }
        if(ev.type == SDL_MOUSEWHEEL)
        {
            window->input.wheel = true;
            window->input.wheel_value = ev.wheel.y;
        }
        if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            window->input.mouse_down = true;
            window->input.mouse_button = ev.button.button;
        }
    }
}