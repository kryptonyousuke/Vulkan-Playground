#pragma once
#include "vk_instance.h"

typedef struct {
    uint32_t swapchainImageCount;
    VkImage* swapChainImages;
    VkImageView* swapchainImageViews;
} VKSwapchainImages;


extern VkSubpassDependency dependency;


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

extern VKSwapchainImages getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain, uint32_t imageCount, VkFormat imageColorFormat);

extern void destroySwapchain(VkDevice device, VkSwapchainKHR swapchain, VkAllocationCallbacks *pAllocator);

extern VkRenderPass createRenderPass(VkDevice device, VkFormat imageColorFormat);

extern VkFramebuffer* createFramebuffers(VkDevice* device, uint32_t imageCount, VKSwapchainImages* swapchainImages, VkRenderPass* renderPass, VkExtent2D swapExtent);

extern void destroyImageViews(VkDevice device, VKSwapchainImages swapchainImages);

extern void destroyFramebuffers(VkDevice device, VkFramebuffer* frameBuffers, int framebufferCount);

