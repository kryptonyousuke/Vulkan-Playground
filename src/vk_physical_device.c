#include "vk_physical_device.h"
VkPhysicalDevice createPhysicalDevice(VkInstance vulkan_instance){
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vulkan_instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        SDL_Log("Nenhuma GPU com suporte ao Vulkan encontrada!");
        exit(1);
    }
    VkPhysicalDevice devices[deviceCount]; 
    vkEnumeratePhysicalDevices(vulkan_instance, &deviceCount, devices);
    printf("%d dispositivos encontrados\n", deviceCount);
    for (uint32_t i = 0; i < deviceCount; i++) {
        physicalDevice = devices[i];
        break; 
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        SDL_Log("Falha ao selecionar uma GPU adequada!");
        exit(1);
    }
    return physicalDevice;
}