typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;

struct Window
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    float mouseX, mouseY;
    bool running;
};

void InitWindow(Window* window);
void UpdateWindow(Window* window);
void CleanWindow(Window* window);