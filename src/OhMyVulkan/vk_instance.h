#pragma once
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include "vk_utils.h"
#define DEBUG false



VkInstance createVkInstance();