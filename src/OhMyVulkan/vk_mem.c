#include "vk_mem.h"



VKMemory VKMemStoreVertex(VkPhysicalDevice physicalDevice, VkDevice device, const Vertex* vertex, uint16_t vertexSize){
    VkBuffer vertexBuffer;
    
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = vertexSize,
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
        printf("Falha ao criar Vertex Buffer!\n");
        exit(1);
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
    vkMapMemory(device, vertexBufferMemory, 0, vertexSize, 0, &data);
    
    memset(data, 0, memAllocInfo.allocationSize);
    memcpy(data, vertex, vertexSize);
    vkUnmapMemory(device, vertexBufferMemory);
    return (VKMemory) {
        .vertexBuffer = vertexBuffer,
        .vertexBufferMemory = vertexBufferMemory
    };
}
