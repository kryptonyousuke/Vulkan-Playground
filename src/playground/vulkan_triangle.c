#include "vulkan_triangle.h"
#define MAX_FRAMES_IN_FLIGHT 2

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
    vr = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    if (vr != VK_SUCCESS) {
        fprintf(stderr, "Falha ao obter as capacidades da superficie!\n");
        exit(vr);
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
        fprintf(stderr, "Individual queues are not supported.\n");
        exit(9999);
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
    VkSurfaceFormatKHR selectedFormat = chooseSwapSurfaceFormat(formats, formatCount);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(physicalDevice, surface);
    VkSwapchainPresentModesCreateInfoKHR presentModeCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_MODES_CREATE_INFO_KHR,
        .pPresentModes = &presentMode,
        .presentModeCount = 1
    };
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = &presentModeCreateInfo,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = selectedFormat.format,
        .imageColorSpace = selectedFormat.colorSpace,
        .imageExtent = swapExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE
    };

    VkSwapchainKHR swapchain = createSwapchain(device, swapchainCreateInfo);
    VKSwapchainImages swapchainImages = getSwapchainImages(device, swapchain, imageCount, selectedFormat.format);
    VkRenderPass renderPass = createRenderPass(device, selectedFormat.format);
    VkFramebuffer* frameBuffers = createFramebuffers(&device, imageCount, &swapchainImages, &renderPass, swapExtent);
    VKPipelineWorktools pipelineWorktools = createPipeline(device, swapExtent, &queueFamilies, &renderPass, MAX_FRAMES_IN_FLIGHT);
    
    const Vertex vertices[] = {
        {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // Top (Red)
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, // Right (Green)
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}  // Left (Blue)
    };
    
    VKMemory vertexPostAllocationData = VKMemStoreVertex(physicalDevice, device, vertices, sizeof(vertices));



    /*
     * Semaphores synchronize execution between GPU queues and the WSI.
     * They are used for GPU-to-GPU and GPU-to-presentation ordering.
     *
     * Fences synchronize GPU execution with the CPU, allowing the CPU
     * to know when submitted work has completed.
     *
     * Semaphore and fence errors are conceptually different and usually
     * indicate incorrect ownership or reuse of synchronization objects.
     */
    VkSemaphore* imageAvailableSemaphores = calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkSemaphore)); // Signals if the GPU can write or read the VkImage (GPU-to-GPU sync);
    VkSemaphore* renderFinishedSemaphores = calloc(imageCount, sizeof(VkSemaphore)); // Signaled by the graphics queue when render commands were finished.
    VkFence* inFlightFences = calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkFence)); // Signals if a frame is being produced (GPU-to-CPU).
    VkFence* presentFences = calloc(MAX_FRAMES_IN_FLIGHT, sizeof(VkFence)); // Optional present queue synchronization (it's not being really used, performance is poor).

    
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        if(vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) == VK_SUCCESS
            && vkCreateFence(device, &fenceInfo, NULL, &inFlightFences[i]) == VK_SUCCESS
            && vkCreateFence(device, &fenceInfo, NULL, &presentFences[i]) == VK_SUCCESS){
            printf("ImageAvailableSemaphore and Fence %d created.\n", i+1);
        } else {
            fprintf(stderr, "Falha ao criar semáforo.\n");
            exit(1);
        }
    }
    for (uint32_t i = 0; i < imageCount; i++){
        if (vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) == VK_SUCCESS){
            printf("Semaphore %d created.\n", i);
        } else {
            fprintf(stderr, "InFlightFence creation failed.\n");
            exit(9999);
        }
    }
    bool running = true;
    uint32_t currentFrame = 0;
    uint32_t imageIndex = 0; 
    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            else if (event.type == SDL_EVENT_WINDOW_RESIZED){
                vkDeviceWaitIdle(device);
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
        
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFences[currentFrame]);


        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(pipelineWorktools.commandBuffers[currentFrame], 0);
        VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        vkBeginCommandBuffer(pipelineWorktools.commandBuffers[currentFrame], &beginInfo);

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo rpBegin = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = renderPass,
            .framebuffer = frameBuffers[imageIndex],
            .renderArea.extent = swapExtent,
            .clearValueCount = 1,
            .pClearValues = &clearColor
        };
        vkCmdBeginRenderPass(pipelineWorktools.commandBuffers[currentFrame], &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(pipelineWorktools.commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineWorktools.graphicsPipeline);
        
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(pipelineWorktools.commandBuffers[currentFrame], 0, 1, &vertexPostAllocationData.vertexBuffer, &offsets[0]);
        VkViewport viewport = {
            .x = 0.0f, .y = 0.0f,
            .width = (float)swapExtent.width,
            .height = (float)swapExtent.height,
            .minDepth = 0.0f, .maxDepth = 1.0f
        };
        vkCmdSetViewport(pipelineWorktools.commandBuffers[currentFrame], 0, 1, &viewport);
        
        VkRect2D scissor = { .offset = {0, 0}, .extent = swapExtent };
        vkCmdSetScissor(pipelineWorktools.commandBuffers[currentFrame], 0, 1, &scissor);
        vkCmdDraw(pipelineWorktools.commandBuffers[currentFrame], 3, 1, 0, 0);
        vkCmdEndRenderPass(pipelineWorktools.commandBuffers[currentFrame]);



        vkEndCommandBuffer(pipelineWorktools.commandBuffers[currentFrame]);
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageAvailableSemaphores[currentFrame], 
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &pipelineWorktools.commandBuffers[currentFrame],
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderFinishedSemaphores[imageIndex]
        };

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

        // VkSwapchainPresentFenceInfoEXT presentFenceInfo = {
        //     .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_FENCE_INFO_EXT,
        //     .swapchainCount = 1,
        //     .pFences = &presentFences[currentFrame],
        // };
        VkSwapchainPresentModeInfoKHR presentModeInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_PRESENT_MODE_INFO_KHR,
            // .pNext = &presentFenceInfo,
            .pPresentModes = &presentMode,
            .swapchainCount = 1,
        };
        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = &presentModeInfo,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderFinishedSemaphores[imageIndex], 
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &imageIndex
        };
        // vkWaitForFences(device, 1, &presentFences[currentFrame], VK_TRUE, UINT64_MAX);
        // vkResetFences(device, 1, &presentFences[currentFrame]);
        vkQueuePresentKHR(presentQueue, &presentInfo);
        
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    vkDeviceWaitIdle(device);
    for (uint32_t i = 0; i < swapchainImages.swapchainImageCount; i++) {
        vkDestroyImageView(device, swapchainImages.swapchainImageViews[i], NULL);
    }
    for (uint32_t i = 0; i < imageCount; i++) {
        vkDestroyFramebuffer(device, frameBuffers[i], NULL);
    }
    free(frameBuffers);
    free(swapchainImages.swapChainImages);
    free(swapchainImages.swapchainImageViews);
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
        vkDestroySemaphore(device, imageAvailableSemaphores[i], NULL);
        vkDestroyFence(device, inFlightFences[i], NULL);
        vkDestroyFence(device, presentFences[i], NULL);
    }
    for (uint32_t i = 0; i < imageCount; i++){
        vkDestroySemaphore(device, renderFinishedSemaphores[i], NULL);
    }
    free(imageAvailableSemaphores);
    free(renderFinishedSemaphores);
    free(inFlightFences);
    free(presentFences);
    vkDestroyCommandPool(device, pipelineWorktools.commandPool, NULL); // command buffer is destroyed here too.
    free(pipelineWorktools.commandBuffers);
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