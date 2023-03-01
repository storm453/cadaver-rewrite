typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

struct Window
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    struct 
    { 
        float mouseX;
        float mouseY;
        bool w;
        bool a;
        bool s;
        bool d;
    }input;
};

void InitWindow(Window* window);
void UpdateWindow(Window* window);
void CleanWindow(Window* window);