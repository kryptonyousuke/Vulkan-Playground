#include "vk_instance.h"

uint32_t sdlExtensionCount = 0;

#ifdef ENABLED_DEBUG

static const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};
#endif
static const char* additionalInstanceExtensions[] = {
    "VK_EXT_surface_maintenance1",
    "VK_KHR_get_surface_capabilities2"
};

VkApplicationInfo appInfo = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "Vulkan Triangle",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "No Engine",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_4
};
VulkanInstance createVkInstance(){
    // Enumerate supported instance extensions
    uint32_t vkSupportedInstanceExtensionsCount;
    vkEnumerateInstanceExtensionProperties(NULL, &vkSupportedInstanceExtensionsCount, NULL);
    VkExtensionProperties* vkSupportedInstanceExtensions = calloc(vkSupportedInstanceExtensionsCount, sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(NULL, &vkSupportedInstanceExtensionsCount, vkSupportedInstanceExtensions);
    printf("Supported instance extensions:\n");
    for (uint32_t i = 0; i < vkSupportedInstanceExtensionsCount; i++) {
        printf("\t %s (Versão: %u)\n", vkSupportedInstanceExtensions[i].extensionName, vkSupportedInstanceExtensions[i].specVersion);
    }
    free(vkSupportedInstanceExtensions);
    
    // Gets the extensions required by SDL3 and adds our required extensions
    uint32_t sdlExtensionCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    uint32_t additionalCount = sizeof(additionalInstanceExtensions) / sizeof(additionalInstanceExtensions[0]);
    uint32_t totalCount = sdlExtensionCount + additionalCount;
    
    
    const char** allExtensions = calloc(totalCount, sizeof(const char*));
    for (uint32_t i = 0; i < sdlExtensionCount; i++)                 allExtensions[i] = sdlExtensions[i];
    for (uint32_t i = 0; i < additionalCount; i++)                   allExtensions[sdlExtensionCount + i] = additionalInstanceExtensions[i];
    
    // Shows the activated extensions
    printf("Activated instance extensions:\n");
    for (uint32_t i = 0; i < totalCount; i++){
        printf("        %s\n", allExtensions[i]);
    }

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = totalCount,
        .ppEnabledExtensionNames = allExtensions,
#ifdef ENABLED_DEBUG
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = validationLayers,
#else
        .enabledLayerCount = 0,
#endif
        .pNext = NULL
    };



    VkInstance vulkan_instance;
    VkResult result = vkCreateInstance(&createInfo, NULL, &vulkan_instance);

    if (result != VK_SUCCESS){
        fprintf(stderr, "Erro no vkCreateInstance: %d\n", result);
        exit(1);
    }
    fprintf(stdout, "Instancia criada em: %p\n", (void*)vulkan_instance);
    return (VulkanInstance){
        .instance = vulkan_instance,
        .instanceExtensions = allExtensions
    };
}

void destroyInstance(VulkanInstance vkInstance){
    free((void*)vkInstance.instanceExtensions); // explicit cast just to silence fool warnings
    vkDestroyInstance(vkInstance.instance, VK_NULL_HANDLE);
}