#include "vk_logical_device.h"

static const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_SWAPCHAIN_MAINTENANCE_1_EXTENSION_NAME
};

VkPhysicalDeviceSwapchainMaintenance1FeaturesEXT swapchainMaint = {
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SWAPCHAIN_MAINTENANCE_1_FEATURES_EXT,
    .swapchainMaintenance1 = VK_TRUE
};



VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkDeviceQueueCreateInfo* queueCreateInfos, uint32_t queueCount){
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);
    VkExtensionProperties* availableExtensions = calloc(extensionCount, sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, availableExtensions);
    printf("Extensões suportadas pelo Device:\n");
    for (uint32_t i = 0; i < extensionCount; i++) {
        printf("\t %s (Versão: %u)\n", availableExtensions[i].extensionName, availableExtensions[i].specVersion);
    }
    free(availableExtensions);
    VkPhysicalDeviceFeatures2 deviceFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &swapchainMaint
    };
    vkGetPhysicalDeviceFeatures2(physicalDevice, &deviceFeatures);

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = queueCount,
        .pQueueCreateInfos = queueCreateInfos,
        .enabledExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]),
        .ppEnabledExtensionNames = deviceExtensions,
        .pNext = &deviceFeatures
    };
    
    if (swapchainMaint.swapchainMaintenance1) {
        printf("Swapchain Maintenance 1 supported\n");
    } else {
        printf("Swapchain Maintance NOT supported\n");
    }
    VkDevice device;
    vr = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);
    if (vr != VK_SUCCESS) {
        fprintf(stderr, "Falha ao criar o Dispositivo Lógico! (ERRNO %d)\n", vr);
        exit(1);
    }
    return device;
}