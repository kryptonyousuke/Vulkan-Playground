#include "vulkan_triangle.h"
#include "src/vk_swapchain.h"
#include <string.h>
#include <vulkan/vulkan_core.h>

/*

    Structures for vulkan initialization and use.

*/


const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};




VkPipelineRasterizationStateCreateInfo rasterizer = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0f,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = VK_FALSE
};
VkPipelineMultisampleStateCreateInfo multisampling = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = VK_FALSE,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
};
VkPipelineColorBlendAttachmentState colorBlendAttachment = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = VK_FALSE
};

VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE
};

VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
};

// Define como os dados estão organizados na memória
VkVertexInputBindingDescription bindingDescription = {
    .binding = 0,
    .stride = sizeof(float) * 5, // Ex: 3 floats para posição + 2 para UV (ou 3 para cor)
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
};
// Define o que cada "Location" no Shader recebe
VkVertexInputAttributeDescription attributeDescriptions[1] = {
    {
        .binding = 0,
        .location = 0, // <--- Isto resolve o erro do Location 0!
        .format = VK_FORMAT_R32G32B32_SFLOAT, // vec3 (x, y, z)
        .offset = 0
    }
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

    VkPhysicalDeviceFeatures deviceFeatures = {0};

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = queueCount,
        .pQueueCreateInfos = queueCreateInfos,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = deviceExtensions,
        .pEnabledFeatures = &deviceFeatures
    };
    VkDevice device;
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device) != VK_SUCCESS) {
        fprintf(stderr, "Falha ao criar o Dispositivo Lógico!");
        return -1;
    }
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

    size_t vertSize, fragSize;
    char* vertCode = readFile("./mesh_basic.vert.spv", &vertSize);
    char* fragCode = readFile("./mesh_basic.frag.spv", &fragSize);
    VkShaderModule vertModule = createShaderModule(device, vertCode, vertSize);
    VkShaderModule fragModule = createShaderModule(device, fragCode, fragSize);
    free(vertCode);
    free(fragCode);
    VkPipelineShaderStageCreateInfo vertStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertModule,
        .pName = "main" // Nome da função de entrada no seu GLSL
    };

    VkPipelineShaderStageCreateInfo fragStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragModule,
        .pName = "main"
    };


    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = 1,
        .pVertexAttributeDescriptions = attributeDescriptions
    };


    VkViewport viewport = {
        .x = 0.0f, .y = 0.0f,
        .width = (float)swapExtent.width,
        .height = (float)swapExtent.height,
        .minDepth = 0.0f, .maxDepth = 1.0f
    };

    VkRect2D scissor = { .offset = {0, 0}, .extent = swapExtent };

    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    
    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    VkPipelineLayout pipelineLayout;
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        SDL_Log("Erro ao criar Pipeline Layout!");
        return -1;
    }
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertStageInfo, fragStageInfo};
    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages, // <--- AQUI entram os shaders!
        
        // ... aqui entram as outras structs (VertexInput, Rasterizer, etc) ...
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &colorBlending,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0
    };

    VkPipeline graphicsPipeline;
    vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline);
    vkDestroyShaderModule(device, vertModule, NULL);
    vkDestroyShaderModule(device, fragModule, NULL);
    // Precisamos encontrar o índice da família de filas que suporta gráficos
// Você provavelmente já fez isso para criar o Device, use o mesmo índice aqui.
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilies.graphicsFamily
    };

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        SDL_Log("Falha ao criar Command Pool!");
        return -1;
    }
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        SDL_Log("Falha ao alocar command buffer!");
    }
    typedef struct {
        float pos[2];
        float color[3];
    } Vertex;

    const Vertex vertices[] = {
        {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // Topo (Vermelho)
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, // Direita (Verde)
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}  // Esquerda (Azul)
    };
    VkBuffer vertexBuffer;
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(vertices),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    VkExternalMemoryBufferCreateInfo externalInfo = {
        .sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT 
    };

    bufferInfo.pNext = &externalInfo;
    if (vkCreateBuffer(device, &bufferInfo, NULL, &vertexBuffer) != VK_SUCCESS) {
        SDL_Log("Falha ao criar Vertex Buffer!");
        return -1;
    }
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo memAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };

    VkDeviceMemory vertexBufferMemory;
    
    vkAllocateMemory(device, &memAllocInfo, NULL, &vertexBufferMemory);

    // "Colar" a memória ao buffer
    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);
    void* data;
    vkMapMemory(device, vertexBufferMemory, 0, sizeof(vertices), 0, &data);
    
    memset(data, 0, memAllocInfo.allocationSize);
    memcpy(data, vertices, sizeof(vertices));
    vkUnmapMemory(device, vertexBufferMemory);
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
        SDL_Log("Falha ao criar objetos de sincronização!");
        return -1;
    }
    bool running = true;
    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFence);

        // 3. Pedir uma imagem da Swapchain
        uint32_t imageIndex;
        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

        // 4. ABRIR A GRAVAÇÃO (Resolve o seu erro!)
        vkResetCommandBuffer(commandBuffer, 0);
        VkCommandBufferBeginInfo beginInfo = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
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
        vkCmdBeginRenderPass(commandBuffer, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
        // 6. COMANDOS DE DESENHO (Agora sim eles funcionam!)
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offsets[0]);
        
        vkCmdDraw(commandBuffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(commandBuffer);

        // 7. FECHAR A GRAVAÇÃO
        vkEndCommandBuffer(commandBuffer);
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageAvailableSemaphore, // Espera a imagem estar pronta
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
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
        vkQueueWaitIdle(presentQueue);
    }
    for (uint32_t i = 0; i < swapchainImages.swapchainImageCount; i++) {
        vkDestroyImageView(device, swapchainImages.swapchainImageViews[i], NULL);
    }
    for (uint32_t i = 0; i < imageCount; i++) {
        vkDestroyFramebuffer(device, frameBuffers[i], NULL);
    }
    free(frameBuffers);
    free(swapchainImages.swapChainImages);
    vkDestroySemaphore(device, imageAvailableSemaphore, NULL);
    vkDestroySemaphore(device, renderFinishedSemaphore, NULL);
    vkDestroyFence(device, inFlightFence, NULL);
    vkDestroyCommandPool(device, commandPool, NULL); // command buffer is destroyed here too.
    vkDestroyBuffer(device, vertexBuffer, NULL);
    vkFreeMemory(device, vertexBufferMemory, NULL);
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    vkDestroyPipeline(device, graphicsPipeline, NULL);
    vkDestroyRenderPass(device, renderPass, NULL);
    destroySwapchain(device, swapchain, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(vulkan_instance, surface, NULL);
    vkDestroyInstance(vulkan_instance, NULL);
    SDL_DestroyWindow(window.window);
    SDL_Quit();
    return 0;
}