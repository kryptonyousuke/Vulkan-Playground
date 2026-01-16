#include "vk_swapchain.h"

VkSubpassDependency dependency = {
    .srcSubpass = VK_SUBPASS_EXTERNAL,
    .dstSubpass = 0,
    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .srcAccessMask = 0,
    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
};

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

VKSwapchainImages getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain, uint32_t imageCount, VkFormat imageColorFormat){
    printf("Getting SwapChain images...\n");
    uint32_t swapchainImageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL); // Pega a quantidade
    VkImage* swapChainImages = malloc(sizeof(VkImage) * imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapChainImages); // Pega os handles
    VkImageView* swapchainImageViews = malloc(sizeof(VkImageView) * imageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = imageColorFormat,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        if (vkCreateImageView(device, &viewInfo, NULL, &swapchainImageViews[i]) != VK_SUCCESS) {
            printf("Erro ao criar ImageView %d\n", i);
            exit(1);
        }
    }
    VKSwapchainImages swapchainImages = {
        .swapChainImages = swapChainImages,
        .swapchainImageCount = swapchainImageCount,
        .swapchainImageViews = swapchainImageViews
    };
    return swapchainImages;
}


void destroySwapchain(VkDevice device, VkSwapchainKHR swapchain, VkAllocationCallbacks *pAllocator){
    vkDestroySwapchainKHR(device, swapchain, pAllocator);
}

VkRenderPass createRenderPass(VkDevice device, VkFormat imageColorFormat){
    VkAttachmentDescription colorAttachment = {
        .format = imageColorFormat,        // O formato SRGB que você escolheu
        .samples = VK_SAMPLE_COUNT_1_BIT,       // Sem anti-aliasing (MSAA) por enquanto
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,  // Limpar a tela antes de desenhar
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,// Salvar o que desenhamos para ver na tela
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,     // Não importa o layout anterior
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  // Layout pronto para ser exibido na tela
    };
    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0, // Índice no array de anexos
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL // Layout durante o desenho
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef
    };
    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        SDL_Log("Falha ao criar o Render Pass!");
        exit(1);
    }
    return renderPass;
}

VkFramebuffer* createFramebuffers(VkDevice* device, uint32_t imageCount, VKSwapchainImages* swapchainImages, VkRenderPass* renderPass, VkExtent2D swapExtent){
    VkFramebuffer* swapchainFramebuffers = malloc(sizeof(VkFramebuffer) * imageCount);

    for (size_t i = 0; i < imageCount; i++) {
        VkImageView attachments[] = { swapchainImages->swapchainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = *renderPass,    // <--- O mesmo Render Pass para todos!
            .attachmentCount = 1,
            .pAttachments = attachments, // <--- A imagem específica deste frame
            .width = swapExtent.width,
            .height = swapExtent.height,
            .layers = 1
        };

        vkCreateFramebuffer(*device, &framebufferInfo, NULL, &swapchainFramebuffers[i]);
    }
    return swapchainFramebuffers;
}