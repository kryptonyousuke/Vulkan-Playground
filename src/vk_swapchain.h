#pragma once
#include "vulkan_triangle.h"


/*
* Choose an present mode for a swapchain (defines the swapchain length).
* @param device Device
* @param surface Surface
*/
extern VkPresentModeKHR chooseSwapPresentMode(VkPhysicalDevice device, VkSurfaceKHR surface);

/*
* Choose an 2D extent for a swapchain.
*
* @param capabilities Surface capabilities
* @param windowWidth Window width
* @param windowHeight Window height
*/
extern VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, int windowWidth, int windowHeight);

/*
* Choose a surface format for a swapchain.
* @param formats Pointer for supported surface formats
* @param count The count of formats
*/
extern VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* formats, uint32_t count);

extern VkSwapchainKHR createSwapchain(VkDevice device, VkSwapchainCreateInfoKHR swapchainCreateInfo);

extern void destroySwapchain(VkDevice device, VkSwapchainKHR swapchain, VkAllocationCallbacks *pAllocator);

