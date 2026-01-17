#pragma once
#include "vk_instance.h"
extern VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo* queueCreateInfos, uint32_t queueCount);