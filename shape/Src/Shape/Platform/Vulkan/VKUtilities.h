#pragma once

#include "VK.h"
#include "Shape/Graphics/RHI/Definitions.h"
#include <vulkan/vk_mem_alloc.h>
#define VK_CHECK_RESULT(f)                                                                                                                        \
    {                                                                                                                                             \
        VkResult res = (f);                                                                                                                       \
        if(res != VK_SUCCESS)                                                                                                                     \
        {                                                                                                                                         \
            SHAPE_LOG_ERROR("[VULKAN] : VkResult is {0} in {1} at line {2}", Shape::Graphics::VKUtilities::ErrorString(res), __FILE__, __LINE__); \
        }                                                                                                                                         \
    }

namespace Shape
{
	namespace Graphics
	{
		namespace VKUtilities
		{
            VkCommandBuffer BeginSingleTimeCommands();
            void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

            void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
            void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

            void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels = 1, uint32_t layerCount = 1, VkCommandBuffer commandBuffer = nullptr);

            bool IsPresentModeSupported(const std::vector<VkPresentModeKHR>& supportedModes, VkPresentModeKHR presentMode);
            VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& supportedModes, bool vsync);

			std::string ErrorString(VkResult errorCode);

            VkFormat FormatToVK(const RHIFormat format, bool srgb = false);

            VkSamplerAddressMode TextureWrapToVK(const TextureWrap format);
            VkFilter TextureFilterToVK(const TextureFilter filter);
		}
	}
}