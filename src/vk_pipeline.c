#include "vk_pipeline.h"
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

VkPipelineColorBlendAttachmentState colorBlendAttachment = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = VK_FALSE
};

VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
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


VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE
};



VKPipelineWorktools createPipeline(VkDevice* device, VkExtent2D swapExtent, QueueFamilyIndices* queueFamilies, VkRenderPass* renderPass){
    size_t vertSize, fragSize;
    char* vertCode = readFile("./mesh_basic.vert.spv", &vertSize);
    char* fragCode = readFile("./mesh_basic.frag.spv", &fragSize);
    VkShaderModule vertModule = createShaderModule(*device, vertCode, vertSize);
    VkShaderModule fragModule = createShaderModule(*device, fragCode, fragSize);
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
    if (vkCreatePipelineLayout(*device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        SDL_Log("Erro ao criar Pipeline Layout!");
        exit(1);
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
        .renderPass = *renderPass,
        .subpass = 0
    };

    VkPipeline graphicsPipeline;
    vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline);
    vkDestroyShaderModule(*device, vertModule, NULL);
    vkDestroyShaderModule(*device, fragModule, NULL);
    // Precisamos encontrar o índice da família de filas que suporta gráficos
// Você provavelmente já fez isso para criar o Device, use o mesmo índice aqui.
    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilies->graphicsFamily
    };

    VkCommandPool commandPool;
    if (vkCreateCommandPool(*device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        SDL_Log("Falha ao criar Command Pool!");
        exit(1);
    }
    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };
    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(*device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        SDL_Log("Falha ao alocar command buffer!");
    }
    return (VKPipelineWorktools){
      .graphicsPipeline = graphicsPipeline,
      .commandBuffer = commandBuffer,
      .commandPool = commandPool,
      .pipelineLayout = pipelineLayout
    };
}