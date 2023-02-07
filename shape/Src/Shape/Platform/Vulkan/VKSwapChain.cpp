#include "hzpch.h"
#include "VKSwapChain.h"
#include "VKUtilities.h"

namespace Shape
{
	namespace Graphics
	{
		VKSwapChain::VKSwapChain(uint32_t width, uint32_t height)
		{
			m_Width = width;
			m_Height = height;
			m_SwapChain = VK_NULL_HANDLE;
			m_OldSwapChain = VK_NULL_HANDLE;
			m_Surface = VK_NULL_HANDLE;

			// Initialised by first Image Aquire
			m_CurrentBuffer = 0;
			m_AcquireImageIndex = std::numeric_limits<uint32_t>::max();
		}

		VKSwapChain::~VKSwapChain()
		{

		}

		/// <summary>
		/// 初始化交换链，并且获得surface
		/// </summary>
		/// <param name="vsync"></param>
		/// <param name="windowHandle"></param>
		/// <returns></returns>
		bool VKSwapChain::Init(bool vsync, Window* windowHandle)
		{
			SHAPE_PROFILE_FUNCTION();
			m_VSyncEnabled = vsync;

			if (m_Surface == VK_NULL_HANDLE)
				m_Surface = CreatePlatformSurface(VKContext::GetVKInstance(), windowHandle);

			bool success = Init(m_VSyncEnabled);
			return success;
		}

		bool VKSwapChain::Init(bool vsync)
		{
			SHAPE_PROFILE_FUNCTION();
			FindImageFormatAndColourSpace();

			if (!m_Surface)
			{
				SHAPE_LOG_CRITICAL("[VULKAN] Failed to create window surface!");
			}

			VkBool32 queueIndexSupported;
			vkGetPhysicalDeviceSurfaceSupportKHR(VKDevice::Get().GetPhysicalDevice()->GetHandle(), VKDevice::Get().GetPhysicalDevice()->GetGraphicsQueueFamilyIndex(), m_Surface, &queueIndexSupported);

			if (queueIndexSupported == VK_FALSE)
				SHAPE_LOG_ERROR("Present Queue not supported");

			// Swap chain
			VkSurfaceCapabilitiesKHR surfaceCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VKDevice::Get().GetGPU(), m_Surface, &surfaceCapabilities);

			uint32_t numPresentModes;
			vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().GetGPU(), m_Surface, &numPresentModes, VK_NULL_HANDLE);

			std::vector<VkPresentModeKHR> pPresentModes(numPresentModes);
			vkGetPhysicalDeviceSurfacePresentModesKHR(VKDevice::Get().GetGPU(), m_Surface, &numPresentModes, pPresentModes.data());

			VkExtent2D swapChainExtent;

			swapChainExtent.width = static_cast<uint32_t>(m_Width);
			swapChainExtent.height = static_cast<uint32_t>(m_Height);

			VkPresentModeKHR swapChainPresentMode = VKUtilities::ChoosePresentMode(pPresentModes, vsync);

			// Use triple-buffering
			m_SwapChainBufferCount = surfaceCapabilities.maxImageCount;
			if (m_SwapChainBufferCount > 3)
				m_SwapChainBufferCount = 3;

			VkSurfaceTransformFlagBitsKHR preTransform;
			if(surfaceCapabilities.supportedTransforms&VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
				preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			else
				preTransform = surfaceCapabilities.currentTransform;

			VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
				VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
				VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
				VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
				VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
			};
			for (auto& compositeAlphaFlag : compositeAlphaFlags)
			{
				if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag)
				{
					compositeAlpha = compositeAlphaFlag;
					break;
				};
			}

			VkSwapchainCreateInfoKHR swapChainCI{};
			swapChainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapChainCI.surface = m_Surface;
			swapChainCI.minImageCount = m_SwapChainBufferCount;
			swapChainCI.imageFormat = m_ColourFormat;
			swapChainCI.imageExtent.width = swapChainExtent.width;
			swapChainCI.imageExtent.height = swapChainExtent.height;
			swapChainCI.preTransform = preTransform;
			swapChainCI.compositeAlpha = compositeAlpha;
			swapChainCI.imageArrayLayers = 1;
			swapChainCI.presentMode = swapChainPresentMode;
			swapChainCI.oldSwapchain = m_OldSwapChain;
			swapChainCI.imageColorSpace = m_ColourSpace;
			swapChainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapChainCI.queueFamilyIndexCount = 0;
			swapChainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			swapChainCI.pQueueFamilyIndices = VK_NULL_HANDLE;
			swapChainCI.clipped = VK_TRUE;

			if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
			{
				swapChainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			}

			if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			{
				swapChainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}

			VK_CHECK_RESULT(vkCreateSwapchainKHR(VKDevice::Get().GetDevice(), &swapChainCI, VK_NULL_HANDLE, &m_SwapChain));

			//创建交换链完成
			//if (m_OldSwapChain != VK_NULL_HANDLE)
			//{
			//	for (uint32_t i = 0; i < m_SwapChainBufferCount; i++)
			//	{
			//		if (m_Frames[i].MainCommandBuffer->GetState() == CommandBufferState::Submitted)
			//			m_Frames[i].MainCommandBuffer->Wait();

			//		m_Frames[i].MainCommandBuffer->Reset();

			//		delete m_SwapChainBuffers[i];
			//		vkDestroySemaphore(VKDevice::Get().GetDevice(), m_Frames[i].PresentSemaphore, nullptr);
			//		m_Frames[i].PresentSemaphore = VK_NULL_HANDLE;
			//	}

			//	m_SwapChainBuffers.clear();

			//	vkDestroySwapchainKHR(VKDevice::Get().GetDevice(), m_OldSwapChain, VK_NULL_HANDLE);
			//	m_OldSwapChain = VK_NULL_HANDLE;
			//}

			uint32_t swapChainImageCount;
			VK_CHECK_RESULT(vkGetSwapchainImagesKHR(VKDevice::Get().GetDevice(), m_SwapChain, &swapChainImageCount, VK_NULL_HANDLE));

			VkImage* pSwapChainImages = new VkImage[swapChainImageCount];
			VK_CHECK_RESULT(vkGetSwapchainImagesKHR(VKDevice::Get().GetDevice(), m_SwapChain, &swapChainImageCount, pSwapChainImages));

			for (uint32_t i = 0; i < m_SwapChainBufferCount; i++)
			{
				VkImageViewCreateInfo viewCI{};
				viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				viewCI.format = m_ColourFormat;
				viewCI.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
				viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				viewCI.subresourceRange.baseMipLevel = 0;
				viewCI.subresourceRange.levelCount = 1;
				viewCI.subresourceRange.baseArrayLayer = 0;
				viewCI.subresourceRange.layerCount = 1;
				viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
				viewCI.flags = 0;
				viewCI.image = pSwapChainImages[i];

				VkImageView imageView;
				VK_CHECK_RESULT(vkCreateImageView(VKDevice::Get().GetDevice(), &viewCI, VK_NULL_HANDLE, &imageView));

				//创建 ImageView

				//VKTexture2D* swapChainBuffer = new VKTexture2D(pSwapChainImages[i], imageView, m_ColourFormat, m_Width, m_Height);
				//swapChainBuffer->TransitionImage(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

				//m_SwapChainBuffers.push_back(swapChainBuffer);

			}
			delete[] pSwapChainImages;

			//CreateFrameData();
			return true;
		}

		void VKSwapChain::FindImageFormatAndColourSpace()
		{
			SHAPE_PROFILE_FUNCTION();
			VkPhysicalDevice physicalDevice = VKDevice::Get().GetPhysicalDevice()->GetHandle();
			// Get list of supported surface formats
			uint32_t formatCount;
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, NULL));
			SHAPE_ASSERT(formatCount > 0, "");

			std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
			VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data()));

			// if the surface format list only includes one entry with VK_FORMAT_UNDEFINED;
			// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
			if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
			{
				m_ColourFormat = VK_FORMAT_B8G8R8A8_UNORM;
				m_ColourSpace = surfaceFormats[0].colorSpace;
			}
			else
			{
				// iterate over the list of available surface format and
				// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
				bool found_B8G8R8A8_UNORM = false;
				for (auto&& surfaceFormat : surfaceFormats)
				{
					if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
					{
						m_ColourFormat = surfaceFormat.format;
						m_ColourSpace = surfaceFormat.colorSpace;
						found_B8G8R8A8_UNORM = true;
						break;
					}
				}

				// in case VK_FORMAT_B8G8R8A8_UNORM is not available
				// select the first available colour format
				if (!found_B8G8R8A8_UNORM)
				{
					m_ColourFormat = surfaceFormats[0].format;
					m_ColourSpace = surfaceFormats[0].colorSpace;
				}
			}

		}

		void VKSwapChain::MakeDefault()
		{
			CreateFunc = CreateFuncVulkan;
		}

		SwapChain* VKSwapChain::CreateFuncVulkan(uint32_t width, uint32_t height)
		{
			return new VKSwapChain(width, height);
		}
	}
}