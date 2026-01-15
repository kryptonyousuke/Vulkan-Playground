#include "vk_instance.h"

uint32_t sdlExtensionCount = 0;
bool enableValidationLayers = DEBUG;
const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

VkApplicationInfo appInfo = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "Vulkan Triangle",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "No Engine",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_4
};
VkInstance createVkInstance(){
    uint32_t sdlExtensionCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

    bool enableValidationLayers = true; 

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = sdlExtensionCount,
        .ppEnabledExtensionNames = sdlExtensions,
        .pNext = NULL
    };

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }
    VkInstance vulkan_instance;
    VkResult result = vkCreateInstance(&createInfo, NULL, &vulkan_instance);
    if (result != VK_SUCCESS){
        fprintf(stderr, "Erro no vkCreateInstance: %d\n", result);
        exit(1);
    }
    fprintf(stdout, "Instancia criada em: %p\n", (void*)vulkan_instance);
    return vulkan_instance;
}