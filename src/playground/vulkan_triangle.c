#include "vulkan_triangle.h"
#include <string.h>

SDL_windowHandle window = {
    .width = 1000,
    .height = 400,
    .title = "Vulkan Triangle"
};

/*

    Main code.

*/

int main(){
    initWindow(&window);
    VkInstance vulkan_instance = createVkInstance();

    
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(window.window, vulkan_instance, NULL, &surface);
    VkPhysicalDevice physicalDevice = createPhysicalDevice(vulkan_instance); 
    VkSurfaceCapabilitiesKHR capabilities;
    VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    if (res != VK_SUCCESS) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Falha ao obter as capacidades da superficie!");
        return -1;
    }

    uint32_t imageCount = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    QueueFamilyIndices queueFamilies = findQueueFamilies(physicalDevice, surface);
    
    VkDeviceQueueCreateInfo queueCreateInfos[2];
    uint32_t queueCount = 0;

    if (queueFamilies.graphicsFamily == queueFamilies.presentFamily) {
        queueCreateInfos[0] = (VkDeviceQueueCreateInfo){
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queueFamilies.graphicsFamily,
            .queueCount = 1,
            .pQueuePriorities = (float[]){1.0f}
        };
        queueCount = 1;
    } else {
        fprintf(stderr, "Filas separadas não são suportadas\n");
        return -1;
    }

    VkDevice device = createLogicalDevice(physicalDevice, queueCreateInfos, queueCount);

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    vkGetDeviceQueue(device, queueFamilies.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, queueFamilies.presentFamily, 0, &presentQueue);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
    VkSurfaceFormatKHR formats[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats);
    VkExtent2D swapExtent = chooseSwapExtent(&capabilities, window.width, window.height);
    // Aqui criamos a variável que o erro disse que faltava
    VkSurfaceFormatKHR selectedFormat = chooseSwapSurfaceFormat(formats, formatCount);

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = selectedFormat.format,
        .imageColorSpace = selectedFormat.colorSpace,
        .imageExtent = swapExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = chooseSwapPresentMode(physicalDevice, surface),
        .clipped = VK_TRUE
    };

    VkSwapchainKHR swapchain = createSwapchain(device, swapchainCreateInfo);
    VKSwapchainImages swapchainImages = getSwapchainImages(device, swapchain, imageCount, selectedFormat.format);
    VkRenderPass renderPass = createRenderPass(device, selectedFormat.format);
    VkFramebuffer* frameBuffers = createFramebuffers(&device, imageCount, &swapchainImages, &renderPass, swapExtent);
    VKPipelineWorktools pipelineWorktools = createPipeline(device, swapExtent, &queueFamilies, &renderPass);
    
    const Vertex vertices[] = {
        {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // Topo (Vermelho)
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, // Direita (Verde)
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}  // Esquerda (Azul)
    };
    
    VKMemory vertexPostAllocationData = VKMemStoreVertex(physicalDevice, device, vertices, sizeof(vertices));

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;

    VkFence inFlightFence;

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT // Começa "aberto" para o primeiro frame não travar
    };

    if (vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, NULL, &inFlightFence) != VK_SUCCESS) {
        printf("Falha ao criar fences ou swapchains!\n");
        return -1;
    }
    bool running = true;
    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_WINDOW_RESIZED){
                window.width = event.window.data1;
                window.height = event.window.data2;
                swapExtent = chooseSwapExtent(&capabilities, window.width, window.height);
                swapchainCreateInfo.imageExtent = swapExtent;
                destroyImageViews(device, swapchainImages);
                destroyFramebuffers(device, frameBuffers, imageCount);
                free(frameBuffers);
                free(swapchainImages.swapChainImages);
                free(swapchainImages.swapchainImageViews);
                destroySwapchain(device, swapchain, NULL);
                swapchain = createSwapchain(device, swapchainCreateInfo);
                swapchainImages = getSwapchainImages(device, swapchain, imageCount, selectedFormat.format);
                frameBuffers = createFramebuffers(&device, imageCount, &swapchainImages, &renderPass, swapExtent);
                
            }
        }
        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFence);

        // 3. Pedir uma imagem da Swapchain
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        // 4. ABRIR A GRAVAÇÃO (Resolve o seu erro!)
        vkResetCommandBuffer(pipelineWorktools.commandBuffer, 0);
        VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        vkBeginCommandBuffer(pipelineWorktools.commandBuffer, &beginInfo);
        // 5. Iniciar o Render Pass (Diz onde desenhar)
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo rpBegin = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = frameBuffers[imageIndex], // Usa o índice que pegamos no passo 3
            .renderArea.extent = swapExtent,
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };
        vkCmdBeginRenderPass(pipelineWorktools.commandBuffer, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
        // 6. COMANDOS DE DESENHO (Agora sim eles funcionam!)
        vkCmdBindPipeline(pipelineWorktools.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineWorktools.graphicsPipeline);
        
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(pipelineWorktools.commandBuffer, 0, 1, &vertexPostAllocationData.vertexBuffer, &offsets[0]);
        
        vkCmdDraw(pipelineWorktools.commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(pipelineWorktools.commandBuffer);
        VkViewport viewport = {
            .x = 0.0f, .y = 0.0f,
            .width = (float)swapExtent.width,
            .height = (float)swapExtent.height,
            .minDepth = 0.0f, .maxDepth = 1.0f
        };
        vkCmdSetViewport(pipelineWorktools.commandBuffer, 0, 1, &viewport);
        
        VkRect2D scissor = { .offset = {0, 0}, .extent = swapExtent };
        vkCmdSetScissor(pipelineWorktools.commandBuffer, 0, 1, &scissor);

        // 7. FECHAR A GRAVAÇÃO
        vkEndCommandBuffer(pipelineWorktools.commandBuffer);
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageAvailableSemaphore, // Espera a imagem estar pronta
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &pipelineWorktools.commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderFinishedSemaphore // Avisa quando acabar de desenhar
        };

        // O 'inFlightFence' garante que a CPU não sobrescreva este buffer enquanto a GPU desenha
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);

        // 2. MOSTRAR NA TELA (PRESENT)
        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderFinishedSemaphore, // Espera o desenho acabar
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex
        };

        vkQueuePresentKHR(presentQueue, &presentInfo);
        // vkQueueWaitIdle(presentQueue);
    }
    for (uint32_t i = 0; i < swapchainImages.swapchainImageCount; i++) {
        vkDestroyImageView(device, swapchainImages.swapchainImageViews[i], NULL);
    }
    for (uint32_t i = 0; i < imageCount; i++) {
        vkDestroyFramebuffer(device, frameBuffers[i], NULL);
    }
    free(frameBuffers);
    free(swapchainImages.swapChainImages);
    free(swapchainImages.swapchainImageViews);
    vkDestroySemaphore(device, imageAvailableSemaphore, NULL);
    vkDestroySemaphore(device, renderFinishedSemaphore, NULL);
    vkDestroyFence(device, inFlightFence, NULL);
    vkDestroyCommandPool(device, pipelineWorktools.commandPool, NULL); // command buffer is destroyed here too.
    vkDestroyBuffer(device, vertexPostAllocationData.vertexBuffer, NULL);
    vkFreeMemory(device, vertexPostAllocationData.vertexBufferMemory, NULL);
    vkDestroyPipelineLayout(device, pipelineWorktools.pipelineLayout, NULL);
    vkDestroyPipeline(device, pipelineWorktools.graphicsPipeline, NULL);
    vkDestroyRenderPass(device, renderPass, NULL);
    destroySwapchain(device, swapchain, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(vulkan_instance, surface, NULL);
    vkDestroyInstance(vulkan_instance, NULL);
    SDL_DestroyWindow(window.window);
    SDL_Quit();
    return 0;
}