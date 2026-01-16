#pragma once
#include "vulkan_triangle.h"

extern VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo* queueCreateInfos, uint32_t queueCount);