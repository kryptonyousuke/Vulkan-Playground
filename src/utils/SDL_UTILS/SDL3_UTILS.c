#include "SDL3_UTILS.h"
void initWindow(SDL_windowHandle* window){
    SDL_Init(SDL_INIT_VIDEO);
    window->window = SDL_CreateWindow(window->title, window->width, window->height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
}
