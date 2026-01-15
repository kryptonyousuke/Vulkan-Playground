#pragma once
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>


typedef struct {
    SDL_Window* window;
    char* title;
    int width;
    int height;
    bool resize;
} SDL_windowHandle;
extern void initWindow(SDL_windowHandle* window);
extern SDL_windowHandle window;