#include "vk_logical_device.h"

static const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};


VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo* queueCreateInfos, uint32_t queueCount){
    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = queueCount,
        .pQueueCreateInfos = queueCreateInfos,
        .enabledExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]),
        .ppEnabledExtensionNames = deviceExtensions,
        .pEnabledFeatures = &deviceFeatures
    };
    VkDevice device;
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device) != VK_SUCCESS) {
        fprintf(stderr, "Falha ao criar o Dispositivo Lógico!");
        exit(1);
    }
    return device;
}