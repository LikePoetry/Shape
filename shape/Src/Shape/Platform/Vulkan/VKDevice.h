#pragma once

#include "Shape/Utilities/TSingleton.h"
#include "VK.h"
#include "VKContext.h"
#include "Shape/Core/StringUtilities.h"
#include "Shape/Graphics/RHI/Definitions.h"

#ifdef USE_VMA_ALLOCATOR
#ifdef SHAPE_DEBUG
#define VMA_DEBUG_MARGIN 16
#define VMA_DEBUG_DETECT_CORRUPTION 1
#endif
#include <vulkan/vk_mem_alloc.h>
#endif

#include <unordered_set>

#include <Tracy/TracyVulkan.hpp>

namespace Shape
{
	namespace Graphics
	{
		class VKPhysicalDevice
		{
		public:
			VKPhysicalDevice();
			~VKPhysicalDevice();

			struct PhysicalDeviceInfo
			{
				std::string GetVendorName()
				{
					std::string name = "Unknown";

					if (VendorID == 0x10DE || StringUtilities::StringContains(Name, "Nvidia"))
					{
						name = "Nvidia";
					}
					else if (VendorID == 0x1002 || VendorID == 0x1022 || StringUtilities::StringContains(Name, "Amd"))
					{
						name = "AMD";
					}
					else if (VendorID == 0x8086 || VendorID == 0x163C || VendorID == 0x8087 || StringUtilities::StringContains(Name, "Intel"))
					{
						name = "Intel";
					}
					else if (VendorID == 0x13B5 || StringUtilities::StringContains(Name, "Arm,"))
					{
						name = "Arm";
					}
					else if (VendorID == 0x5143 || StringUtilities::StringContains(Name, "Qualcomm"))
					{
						name = "Qualcomm";
					}
					else if (VendorID == 0x106b || StringUtilities::StringContains(Name, "Apple"))
					{
						return "Apple";
					}

					return name;
				}

				std::string DecodeDriverVersion(const uint32_t version)
				{
					char buffer[256];

					if (Vendor == "Nvidia")
					{
						sprintf(
							buffer,
							"%d.%d.%d.%d",
							(version >> 22) & 0x3ff,
							(version >> 14) & 0x0ff,
							(version >> 6) & 0x0ff,
							(version) & 0x003f);
					}
					else if (Vendor == "Intel")
					{
						sprintf(
							buffer,
							"%d.%d",
							(version >> 14),
							(version) & 0x3fff);
					}
					else // Vulkan version conventions
					{
						sprintf(
							buffer,
							"%d.%d.%d",
							(version >> 22),
							(version >> 12) & 0x3ff,
							version & 0xfff);
					}

					return buffer;
				}

				uint32_t Memory;
				uint32_t VendorID;
				std::string Driver;
				std::string APIVersion;
				std::string Vendor;
				std::string Name;
				PhysicalDeviceType Type;
				VkPhysicalDevice Handle;
			};

			bool IsExtensionSupported(const std::string& extensionName) const;

			PhysicalDeviceInfo GetInfo(VkPhysicalDevice device);

			VkPhysicalDevice GetHandle() const
			{
				return m_Handle;
			}

		private:
			struct QueueFamilyIndices
			{
				int32_t Graphics = -1;
				int32_t Compute = -1;
				int32_t Transfer = -1;
			};
			QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);

			uint32_t GetGPUCount() const
			{
				return m_GPUCount;
			}

		private:
			QueueFamilyIndices m_QueueFamilyIndices;

			std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
			std::unordered_set<std::string> m_SupportedExtensions;
			std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

			VkPhysicalDevice m_Handle;
			VkPhysicalDeviceProperties m_PhysicalDeviceProperties;
			VkPhysicalDeviceMemoryProperties m_MemoryProperties;

			uint32_t m_GPUCount = 0;
			PhysicalDeviceInfo m_DeviceInfo;

			friend class VKDevice;
		};

		class VKDevice :public ThreadSafeSingleton<VKDevice>
		{
			friend class ThreadSafeSingleton<VKDevice>;

		public:
			VKDevice();
			~VKDevice();

			bool Init();
			void CreatePipelineCache();
			void CreateTracyContext();

			const VkPhysicalDeviceFeatures& GetEnabledFeatures()
			{
				return m_EnabledFeatures;
			}

		private:
			VkDevice m_Device;

			VkQueue m_ComputeQueue;
			VkQueue m_GraphicsQueue;
			VkQueue m_PresentQueue;

			VkPhysicalDeviceFeatures m_EnabledFeatures;

			SharedPtr<VKPhysicalDevice> m_PhysicalDevice;

			bool m_EnableDebugMarkers = false;
		};
	}
}