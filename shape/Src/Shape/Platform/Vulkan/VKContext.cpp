#include "hzpch.h"
#include "VKContext.h"
#include <vulkan/vulkan_win32.h>

#define VK_LAYER_LUNARG_STANDARD_VALIDATION_NAME "VK_LAYER_LUNARG_standard_validation"
#define VK_LAYER_LUNARG_ASSISTENT_LAYER_NAME "VK_LAYER_LUNARG_assistant_layer"
#define VK_LAYER_LUNARG_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

namespace Shape
{
	namespace Graphics
	{
		VkInstance VKContext::s_VkInstance = nullptr;
		uint32_t VKContext::m_VKVersion = 0;

		const std::vector<const char*> VKContext::GetRequiredExtensions()
		{
			std::vector<const char*> extensions;

			if (EnableValidationLayers)
			{
				SHAPE_LOG_INFO("Vulkan : Enabled Validation Layers");
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
				extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
			}

			extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
			extensions.push_back("VK_KHR_portability_enumeration");

			extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

			return extensions;
		}

		const std::vector<const char*> VKContext::GetRequiredLayers() const
		{
			std::vector<const char*> layers;

			if (EnableValidationLayers)
			{
				layers.emplace_back(VK_LAYER_LUNARG_VALIDATION_NAME);
			}

			return layers;
		}

		VKContext::VKContext()
		{

		}

		VKContext::~VKContext()
		{

		}

		void VKContext::Init()
		{
			SHAPE_PROFILE_FUNCTION();
			//创建Vulkan 实例
			CreateInstance();
		}

		/// <summary>
		/// 验证层支持
		/// </summary>
		/// <param name="validationLayers"></param>
		/// <returns></returns>
		bool VKContext::CheckValidationLayerSupport(std::vector<const char*>& validationLayers)
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			m_InstanceLayers.resize(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, m_InstanceLayers.data());
			bool removedLayer = false;

			validationLayers.erase(
				std::remove_if(
					validationLayers.begin(),
					validationLayers.end(),
					[&](const char* layerName)
					{
						bool layerFound = false;

			for (const auto& layerProperties : m_InstanceLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				removedLayer = true;
				SHAPE_LOG_WARN("[VULKAN] Layer not supported - {0}", layerName);
			}

			return !layerFound;
					}),
				validationLayers.end());

			return !removedLayer;
		}

		/// <summary>
		/// 扩展支持
		/// </summary>
		/// <param name="extensions"></param>
		/// <returns></returns>
		bool VKContext::CheckExtensionSupport(std::vector<const char*>& extensions)
		{
			uint32_t extensionCount;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			m_InstanceExtensions.resize(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, m_InstanceExtensions.data());

			bool removedExtension = false;

			extensions.erase(
				std::remove_if(
					extensions.begin(),
					extensions.end(),
					[&](const char* extensionName)
					{
						bool extensionFound = false;

			for (const auto& extensionProperties : m_InstanceExtensions)
			{
				if (strcmp(extensionName, extensionProperties.extensionName) == 0)
				{
					extensionFound = true;
					break;
				}
			}

			if (!extensionFound)
			{
				removedExtension = true;
				SHAPE_LOG_WARN("[VULKAN] Extension not supported - {0}", extensionName);
			}

			return !extensionFound;
					}),
				extensions.end());

			return !removedExtension;
		}

		/// <summary>
		/// 创建VkInstance
		/// </summary>
		void VKContext::CreateInstance()
		{
			SHAPE_PROFILE_FUNCTION();
			if (volkInitialize() != VK_SUCCESS)
			{
				SHAPE_LOG_CRITICAL("volkInitialize failed!");
			}

			if (volkGetInstanceVersion() == 0)
			{
				SHAPE_LOG_CRITICAL("Could not find loader");
			}

			m_InstanceLayerNames = GetRequiredLayers();
			m_InstanceExtensionNames = GetRequiredExtensions();

			if(!CheckValidationLayerSupport(m_InstanceLayerNames))
			{
				SHAPE_LOG_CRITICAL("[VULKAN] Validation layers requested, but not available!");
			}

			if (!CheckExtensionSupport(m_InstanceExtensionNames))
			{
				SHAPE_LOG_CRITICAL("[VULKAN] Extensions requested are not available!");
			}
		}

		void VKContext::MakeDefault()
		{
			CreateFunc = CreateFuncVulkan;
		}

		GraphicsContext* VKContext::CreateFuncVulkan()
		{
			return new VKContext();
		}
	}
}