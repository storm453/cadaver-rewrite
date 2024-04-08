#pragma once

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef void *SDL_GLContext;

struct Window
{
    SDL_GLContext context;
    SDL_Window* window;
    bool running;
    float width = 1920;
    float height = 1080;
    struct
    {
        float mouseX;
        float mouseY;
        bool w;
        bool a;
        bool s;
        bool d;
        bool shift;
    }input;
};

struct Camera
{
    float x = 0;
    float y = 0;
};

void init_window(Window* window);
void update_window(Window* window);
void clean_window(Window* window);