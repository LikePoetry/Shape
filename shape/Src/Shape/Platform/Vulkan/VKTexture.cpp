#include "hzpch.h"
#include "VKTexture.h"

namespace Shape
{
	namespace Graphics
	{
		static VkImageView CreateImageView(VkImage image, VkFormat format, uint32_t mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectMask, uint32_t layerCount, uint32_t baseArrayLayer = 0, uint32_t baseMipLevel = 0)
		{
			SHAPE_PROFILE_FUNCTION();
			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = viewType;
			viewInfo.format = format;
			viewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			viewInfo.subresourceRange.aspectMask = aspectMask;
			viewInfo.subresourceRange.baseMipLevel = baseMipLevel;
			viewInfo.subresourceRange.levelCount = mipLevels;
			viewInfo.subresourceRange.baseArrayLayer = baseArrayLayer;
			viewInfo.subresourceRange.layerCount = layerCount;

			VkImageView imageView;
			if (vkCreateImageView(VKDevice::Get().GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				SHAPE_LOG_ERROR("Failed to create texture image view!");
			}

			return imageView;
		}

		static VkSampler CreateTextureSampler(VkFilter magFilter = VK_FILTER_LINEAR, VkFilter minFilter = VK_FILTER_LINEAR, float minLod = 0.0f, float maxLod = 1.0f, bool anisotropyEnable = false, float maxAnisotropy = 1.0f, VkSamplerAddressMode modeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VkSamplerAddressMode modeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE)
		{
			SHAPE_PROFILE_FUNCTION();
			VkSampler sampler;
			VkSamplerCreateInfo samplerInfo = {};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = magFilter;
			samplerInfo.minFilter = minFilter;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.addressModeU = modeU;
			samplerInfo.addressModeV = modeV;
			samplerInfo.addressModeW = modeW;
			samplerInfo.maxAnisotropy = 1;     // maxAnisotropy;
			samplerInfo.anisotropyEnable = false; // anisotropyEnable;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
			samplerInfo.minLod = minLod;
			samplerInfo.maxLod = maxLod;

			if (vkCreateSampler(VKDevice::Get().GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
			{
				SHAPE_LOG_ERROR("Failed to create texture sampler!");
			}

			return sampler;
		}

#ifdef USE_VMA_ALLOCATOR
		static void CreateImageVma(const VkImageCreateInfo& imageInfo, VkImage& image, VmaAllocation& allocation)
		{
			SHAPE_PROFILE_FUNCTION();
			VmaAllocationCreateInfo allocInfovma;
			allocInfovma.flags = 0;
			allocInfovma.usage = VMA_MEMORY_USAGE_AUTO;
			allocInfovma.requiredFlags = 0;
			allocInfovma.preferredFlags = 0;
			allocInfovma.memoryTypeBits = 0;
			allocInfovma.pool = nullptr;
			allocInfovma.pUserData = nullptr;
			vmaCreateImage(VKDevice::Get().GetAllocator(), &imageInfo, &allocInfovma, &image, &allocation, nullptr);
		}
#endif

#ifdef USE_VMA_ALLOCATOR
		static void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageType imageType, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, uint32_t arrayLayers, VkImageCreateFlags flags, VmaAllocation& allocation)
#endif
		{
			SHAPE_PROFILE_FUNCTION();
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = imageType;
			imageInfo.extent = { width, height, 1 };
			imageInfo.mipLevels = mipLevels;
			imageInfo.format = format;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = usage;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.arrayLayers = arrayLayers;

			imageInfo.flags = flags;

#ifdef USE_VMA_ALLOCATOR
			CreateImageVma(imageInfo, image, allocation);
#else
#endif
		}

		VKTexture2D::VKTexture2D(TextureDesc parameters, uint32_t width, uint32_t height)
			: m_FileName("NULL")
			, m_TextureImage(VK_NULL_HANDLE)
			, m_TextureImageView(VK_NULL_HANDLE)
			, m_TextureSampler(VK_NULL_HANDLE)
		{
			m_Width = width;
			m_Height = height;
			m_Parameters = parameters;
			m_VKFormat = VKUtilities::FormatToVK(parameters.format, parameters.srgb);
			m_DeleteImage = true;
			m_MipLevels = 1;
			m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);
			m_Flags = m_Parameters.flags;

			BuildTexture();
		}
	}
}