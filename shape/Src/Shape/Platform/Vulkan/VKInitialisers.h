#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace Shape
{
	namespace VKInitialisers
	{
        inline VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
            VkCommandPool commandPool,
            VkCommandBufferLevel level,
            uint32_t bufferCount)
        {
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool = commandPool;
            commandBufferAllocateInfo.level = level;
            commandBufferAllocateInfo.commandBufferCount = bufferCount;
            return commandBufferAllocateInfo;
        }

        inline VkSemaphoreCreateInfo SemaphoreCreateInfo()
        {
            VkSemaphoreCreateInfo semaphoreCreateInfo = {};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            return semaphoreCreateInfo;
        }
	}
}