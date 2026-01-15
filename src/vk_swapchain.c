#include "vk_swapchain.h"

VkPresentModeKHR chooseSwapPresentMode(VkPhysicalDevice device, VkSurfaceKHR surface) {
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);

    VkPresentModeKHR availableModes[presentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, availableModes);

    for (uint32_t i = 0; i < presentModeCount; i++) {
        // Mailbox is preferred (Triple Buffering + VSync)
        if (availableModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availableModes[i];
        }
    }

    // FIFO is granted by the driver as a fallback
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, int windowWidth, int windowHeight) {
    // Se o sistema já definiu o tamanho, usamos o que ele mandou
    if (capabilities->currentExtent.width != 0xFFFFFFFF) {
        return capabilities->currentExtent;
    } else {
        // Caso contrário, definimos nós, respeitando os limites da GPU
        VkExtent2D actualExtent = { (uint32_t)windowWidth, (uint32_t)windowHeight };

        actualExtent.width = (actualExtent.width < capabilities->minImageExtent.width) ? capabilities->minImageExtent.width : actualExtent.width;
        actualExtent.width = (actualExtent.width > capabilities->maxImageExtent.width) ? capabilities->maxImageExtent.width : actualExtent.width;

        actualExtent.height = (actualExtent.height < capabilities->minImageExtent.height) ? capabilities->minImageExtent.height : actualExtent.height;
        actualExtent.height = (actualExtent.height > capabilities->maxImageExtent.height) ? capabilities->maxImageExtent.height : actualExtent.height;

        return actualExtent;
    }
}
VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR* formats, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        // Procuramos por cores SRGB e formato BGRA (comum em telas)
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }
    return formats[0]; // Se não achar, pega o primeiro disponível
}
VkSwapchainKHR createSwapchain(VkDevice device, VkSwapchainCreateInfoKHR swapchainCreateInfo){
    VkSwapchainKHR swapChain;
    vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapChain);
    return swapChain;
}
void destroySwapchain(VkDevice device, VkSwapchainKHR swapchain, VkAllocationCallbacks *pAllocator){
    vkDestroySwapchainKHR(device, swapchain, pAllocator);
}
