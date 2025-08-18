#pragma once

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef void *SDL_GLContext;

struct Window
{
    SDL_GLContext context;
    SDL_Window* window;
    bool running;
    float width = 1280;
    float height = 720;
    struct
    {
        int mouseX;
        int mouseY;
        float mouse_down;
        int mouse_button;
        float wheel_value;
        float wheel;
        bool w;
        bool a;
        bool s;
        bool d;
        bool l;
        bool shift;
    }input;
};

void init_window(Window* window);
void update_window(Window* window);