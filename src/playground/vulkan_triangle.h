#pragma once

/*

    It's not needed to use extern in function definitions, but i
    usually prefer turn everything clear as possible.

*/

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../utils/SDL_UTILS/SDL3_UTILS.h"
#include "../utils/sys_interaction.h"
#include "../OhMyVulkan/vk_utils.h"
#include "../OhMyVulkan/vk_instance.h"
#include "../OhMyVulkan/vk_swapchain.h"
#include "../OhMyVulkan/vk_logical_device.h"
#include "../OhMyVulkan/vk_physical_device.h"
#include "../OhMyVulkan/vk_pipeline.h"
#include "../OhMyVulkan/vk_mem.h"
#include "../OhMyVulkan/vk_sync.h"
