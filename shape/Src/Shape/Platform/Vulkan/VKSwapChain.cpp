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