#include "hzpch.h"
#include "VKTexture.h"
#include "VKUtilities.h"
#include "Shape/Utilities/LoadImage.h"
#include "Shape/Maths/Maths.h"
#include "VKBuffer.h"

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

		VKTexture2D::VKTexture2D(uint32_t width, uint32_t height, void* data, TextureDesc parameters, TextureLoadOptions loadOptions)
			: m_FileName("NULL")
			, m_TextureImage(VK_NULL_HANDLE)
			, m_TextureImageView(VK_NULL_HANDLE)
			, m_TextureSampler(VK_NULL_HANDLE)
		{
			m_Width = width;
			m_Height = height;
			m_Parameters = parameters;
			m_LoadOptions = loadOptions;
			m_Data = static_cast<uint8_t*>(data);
			m_Format = parameters.format;
			m_VKFormat = VKUtilities::FormatToVK(parameters.format, parameters.srgb);
			m_Flags = m_Parameters.flags;

			Load();

			m_TextureImageView = Graphics::CreateImageView(m_TextureImage, VKUtilities::FormatToVK(parameters.format, parameters.srgb), m_MipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);
			m_TextureSampler = Graphics::CreateTextureSampler(VKUtilities::TextureFilterToVK(m_Parameters.magFilter), VKUtilities::TextureFilterToVK(m_Parameters.minFilter), 0.0f, static_cast<float>(m_MipLevels), false, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap));

			UpdateDescriptor();
		}

		VKTexture2D::VKTexture2D(const std::string& name, const std::string& filename, TextureDesc parameters, TextureLoadOptions loadOptions)
			: m_FileName(filename)
			, m_TextureImage(VK_NULL_HANDLE)
			, m_TextureImageView(VK_NULL_HANDLE)
			, m_TextureSampler(VK_NULL_HANDLE)
		{
			m_Parameters = parameters;
			m_LoadOptions = loadOptions;
			m_Format = parameters.format;
			m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);
			m_Flags = m_Parameters.flags;

			m_DeleteImage = Load();

			if (!m_DeleteImage)
				return;

			m_TextureImageView = Graphics::CreateImageView(m_TextureImage, m_VKFormat, m_MipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);
			m_TextureSampler = Graphics::CreateTextureSampler(VKUtilities::TextureFilterToVK(m_Parameters.magFilter), VKUtilities::TextureFilterToVK(m_Parameters.minFilter), 0.0f, static_cast<float>(m_MipLevels), false, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap));

			UpdateDescriptor();
		}

		VKTexture2D::VKTexture2D(VkImage image, VkImageView imageView, VkFormat format, uint32_t width, uint32_t height)
			: m_TextureImage(image)
			, m_TextureImageView(imageView)
			, m_TextureSampler(VK_NULL_HANDLE)
			, m_Width(width)
			, m_Height(height)
			, m_VKFormat(format)
			, m_DeleteImage(false)
			, m_ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
		{
			m_TextureImageMemory = VK_NULL_HANDLE;

			UpdateDescriptor();
		}

		VKTexture2D::~VKTexture2D()
		{
			SHAPE_PROFILE_FUNCTION();
			DeleteResources();
		}

		void VKTexture2D::Resize(uint32_t width, uint32_t height)
		{

		}

		void VKTexture2D::DeleteResources()
		{
			/*VKContext::DeletionQueue& deletionQueue = VKRenderer::GetCurrentDeletionQueue();

			if (m_TextureSampler)
			{
				auto sampler = m_TextureSampler;
				deletionQueue.PushFunction([sampler]
					{ vkDestroySampler(VKDevice::GetHandle(), sampler, nullptr); });
			}

			if (m_TextureImageView)
			{
				auto imageView = m_TextureImageView;
				deletionQueue.PushFunction([imageView]
					{ vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr); });
			}

			for (auto& view : m_MipImageViews)
			{
				if (view.second)
				{
					auto imageView = view.second;
					deletionQueue.PushFunction([imageView]
						{ vkDestroyImageView(VKDevice::GetHandle(), imageView, nullptr); });
				}
			}

			m_MipImageViews.clear();

			if (m_DeleteImage)
			{
				auto image = m_TextureImage;
#ifdef USE_VMA_ALLOCATOR
				auto alloc = m_Allocation;
				deletionQueue.PushFunction([image, alloc]
					{ vmaDestroyImage(VKDevice::Get().GetAllocator(), image, alloc); });
#else
				deletionQueue.PushFunction([image]
					{ vkDestroyImage(VKDevice::Get().GetAllocator(), image, nullptr); });
				if (m_TextureImageMemory)
				{
					auto imageMemory = m_TextureImageMemory;
					deletionQueue.PushFunction([imageMemory]
						{ vkFreeMemory(VKDevice::Get().GetAllocator(), imageMemory, nullptr); });
				}
#endif
			}*/
		}

		void VKTexture2D::UpdateDescriptor()
		{
			m_Descriptor.sampler = m_TextureSampler;
			m_Descriptor.imageView = m_TextureImageView;
			m_Descriptor.imageLayout = m_ImageLayout;
		}

		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, uint32_t layer = 0, uint32_t layerCount = 1)
		{
			SHAPE_PROFILE_FUNCTION();
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(VKDevice::Get().GetGPU(), imageFormat, &formatProperties);

			if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
			{
				SHAPE_LOG_ERROR("Texture image format does not support linear blitting!");
			}

			VkCommandBuffer commandBuffer = VKUtilities::BeginSingleTimeCommands();

			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.image = image;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseArrayLayer = layer;
			barrier.subresourceRange.layerCount = layerCount;
			barrier.subresourceRange.levelCount = 1;

			int32_t mipWidth = texWidth;
			int32_t mipHeight = texHeight;

			for (uint32_t i = 1; i < mipLevels; i++)
			{
				barrier.subresourceRange.baseMipLevel = i - 1;
				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier);

				VkImageBlit blit{};
				blit.srcOffsets[0] = { 0, 0, 0 };
				blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
				blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.srcSubresource.mipLevel = i - 1;
				blit.srcSubresource.baseArrayLayer = layer;
				blit.srcSubresource.layerCount = layerCount;
				blit.dstOffsets[0] = { 0, 0, 0 };
				blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
				blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				blit.dstSubresource.mipLevel = i;
				blit.dstSubresource.baseArrayLayer = layer;
				blit.dstSubresource.layerCount = layerCount;

				vkCmdBlitImage(commandBuffer,
					image,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&blit,
					VK_FILTER_LINEAR);

				barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				vkCmdPipelineBarrier(commandBuffer,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					0,
					0,
					nullptr,
					0,
					nullptr,
					1,
					&barrier);

				if (mipWidth > 1)
					mipWidth /= 2;
				if (mipHeight > 1)
					mipHeight /= 2;
			}

			barrier.subresourceRange.baseMipLevel = mipLevels - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(commandBuffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier);

			VKUtilities::EndSingleTimeCommands(commandBuffer);
		}

		/// <summary>
		/// 载入纹理数据
		/// </summary>
		/// <returns></returns>
		bool VKTexture2D::Load()
		{
			SHAPE_PROFILE_FUNCTION();
			uint32_t bits;
			uint8_t* pixels;

			m_Flags |= TextureFlags::Texture_Sampled;

			if (m_Data == nullptr)
			{
				pixels = Shape::LoadImageFromFile(m_FileName, &m_Width, &m_Height, &bits);

				if (pixels == nullptr)
					return false;

				m_Parameters.format = BitsToFormat(bits);
				m_Format = m_Parameters.format;
			}
			else
			{
				if (m_Data == nullptr)
					return false;

				m_BitsPerChannel = GetBitsFromFormat(m_Parameters.format);
				bits = m_BitsPerChannel;
				pixels = m_Data;
			}

			m_VKFormat = VKUtilities::FormatToVK(m_Parameters.format, m_Parameters.srgb);

			VkDeviceSize imageSize = VkDeviceSize(m_Width * m_Height * bits / 8);

			if (!pixels)
			{
				SHAPE_LOG_CRITICAL("failed to load texture image!");
			}

			m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(Maths::Max(m_Width, m_Height)))) + 1;

			if (!(m_Flags & TextureFlags::Texture_CreateMips))
				m_MipLevels = 1;

			VKBuffer* stagingBuffer = new VKBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, static_cast<uint32_t>(imageSize), pixels);

			if (m_Data == nullptr)
				delete[] pixels;
#ifdef USE_VMA_ALLOCATOR
			Graphics::CreateImage(m_Width, m_Height, m_MipLevels, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0, m_Allocation);
#else
			Graphics::CreateImage(m_Width, m_Height, m_MipLevels, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0);
#endif

			VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_MipLevels);
			VKUtilities::CopyBufferToImage(stagingBuffer->GetBuffer(), m_TextureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height));

			delete stagingBuffer;

			if (m_Flags & TextureFlags::Texture_CreateMips)
				GenerateMipmaps(m_TextureImage, m_VKFormat, m_Width, m_Height, m_MipLevels);

			TransitionImage(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			return true;
		}

		void VKTexture2D::BuildTexture()
		{
			if (m_Flags & TextureFlags::Texture_CreateMips)
			{
				m_MipLevels = static_cast<uint32_t>(std::floor(std::log2(Maths::Max(m_Width, m_Height)))) + 1;
			}

#ifdef USE_VMA_ALLOCATOR
			Graphics::CreateImage(m_Width, m_Height, m_MipLevels, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0, m_Allocation);
#else
			Graphics::CreateImage(m_Width, m_Height, m_MipLevels, m_VKFormat, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory, 1, 0);
#endif

			m_TextureImageView = Graphics::CreateImageView(m_TextureImage, m_VKFormat, m_MipLevels, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1);
			m_TextureSampler = Graphics::CreateTextureSampler(VKUtilities::TextureFilterToVK(m_Parameters.minFilter), VKUtilities::TextureFilterToVK(m_Parameters.magFilter), 0.0f, static_cast<float>(m_MipLevels), false, VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.maxSamplerAnisotropy, VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap), VKUtilities::TextureWrapToVK(m_Parameters.wrap));

			m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			TransitionImage(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

			UpdateDescriptor();

			if (m_Flags & TextureFlags::Texture_MipViews)
			{
				for (uint32_t i = 0; i < m_MipLevels; i++)
				{
					GetMipImageView(i);
				}
			}
		}

		void VKTexture2D::TransitionImage(VkImageLayout newLayout, VKCommandBuffer* commandBuffer)
		{
			SHAPE_PROFILE_FUNCTION();
			if (newLayout != m_ImageLayout)
				VKUtilities::TransitionImageLayout(m_TextureImage, m_VKFormat, m_ImageLayout, newLayout, m_MipLevels, 1, commandBuffer ? commandBuffer->GetHandle() : nullptr);
			m_ImageLayout = newLayout;
			UpdateDescriptor();
		}

		VkImageView VKTexture2D::GetMipImageView(uint32_t mip)
		{
			if (m_MipImageViews.find(mip) == m_MipImageViews.end())
			{
				m_MipImageViews[mip] = CreateImageView(m_TextureImage, m_VKFormat, 1, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT, 1, 0, mip);
			}
			return m_MipImageViews.at(mip);
		}
	}
}