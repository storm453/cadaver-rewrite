#pragma once

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

struct Window
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    float width = 1280;
    float height = 720;
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