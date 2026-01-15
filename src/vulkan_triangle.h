#pragma once

/*

    It's not needed to use extern in function definitions, but i
    usually prefer turn everything clear as possible.

*/

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>
#include "SDL_UTILS/SDL3_UTILS.h"
#include <vulkan/vulkan.h>
#include <stdio.h>
#include <vulkan/vulkan_core.h>
#include <stdlib.h>
#include "vk_utils.h"
#include "sys_interaction.h"
#include "vk_instance.h"
#include "vk_swapchain.h"
#include "vk_device.h"
#include "vk_physical_device.h"
#include "vk_pipeline.h"
