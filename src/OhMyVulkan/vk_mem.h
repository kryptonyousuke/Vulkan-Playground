#pragma once
#include "vk_instance.h"
typedef struct {
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
} VKMemory;
extern VKMemory VKMemStoreVertex(VkPhysicalDevice physicalDevice, VkDevice device, const Vertex* vertex, uint16_t vertexSize);