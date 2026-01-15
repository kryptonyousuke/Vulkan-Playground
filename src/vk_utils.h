#pragma once
#include "vulkan_triangle.h"


struct QueueFamilyIndices {
        int graphicsFamily;
        int presentFamily;
};


typedef struct QueueFamilyIndices QueueFamilyIndices;

/*
* Searchs queue families supported by a device.
*
* @param device Device
* @param surface Surface
*/
extern QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

/*
* Creates a shader module
*
* @param device Device
* @param code Pointer to the shader code
* @param size Code size
*/
extern VkShaderModule createShaderModule(VkDevice device, const char* code, size_t size);

/*
* Searchs for specific memory type according to the specified properties for buffer allocation on vram.
*
* @param physicalDevice Physical device
* @param typeFilter Bit mask that defines which kinds of compatible data formats the memory must be able to store.
* @param properties Memory properties (flags).
*/
extern uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);



