#pragma once
#include "vk_instance.h"
struct VKPipelineWorktools {
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffers;
    VkPipelineLayout pipelineLayout;
};
typedef struct VKPipelineWorktools VKPipelineWorktools;
extern VKPipelineWorktools createPipeline(VkDevice device, VkExtent2D swapExtent, QueueFamilyIndices* queueFamilies, VkRenderPass* renderPass, uint32_t commandBuffersNumber);