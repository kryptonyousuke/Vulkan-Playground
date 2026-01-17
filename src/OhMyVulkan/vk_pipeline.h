#pragma once
#include "vk_instance.h"
struct VKPipelineWorktools {
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;;
    VkCommandBuffer commandBuffer;
    VkPipelineLayout pipelineLayout;
};
typedef struct VKPipelineWorktools VKPipelineWorktools;
extern VKPipelineWorktools createPipeline(VkDevice device, VkExtent2D swapExtent, QueueFamilyIndices* queueFamilies, VkRenderPass* renderPass);