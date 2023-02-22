#include "hzpch.h"
#include "VKContext.h"
#include "VKDevice.h"
#include "VKCommandPool.h"
#include "VKCommandBuffer.h"
#include "VKRenderer.h"
#include "Core/Version.h"
#include "Maths/Maths.h"
#include "Core/StringUtilities.h"
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

		VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
		{
			auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));

			if (func != nullptr)
			{
				return func(instance, pCreateInfo, pAllocator, pCallback);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}
		
		VKContext::VKContext()
		{

		}

		VKContext::~VKContext()
		{
			VKRenderer::FlushDeletionQueues();

			vkDestroyDescriptorPool(VKDevice::Get().GetDevice(), VKRenderer::GetDescriptorPool(), VK_NULL_HANDLE);

			if (m_DebugCallback)
				vkDestroyDebugReportCallbackEXT(s_VkInstance, m_DebugCallback, VK_NULL_HANDLE);

			VKDevice::Release();
			vkDestroyInstance(s_VkInstance, nullptr);
		}

		void VKContext::Init()
		{
			SHAPE_PROFILE_FUNCTION();
			//创建Vulkan 实例
			CreateInstance();
			//设备初始化
			VKDevice::Get().Init();
			SetupDebugCallback();
		}

		void VKContext::Present()
		{
		}

		size_t VKContext::GetMinUniformBufferOffsetAlignment() const
		{
			return Graphics::VKDevice::Get().GetPhysicalDevice()->GetProperties().limits.minUniformBufferOffsetAlignment;
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
		/// 设置Vulkan 回调函数
		/// </summary>
		void VKContext::SetupDebugCallback()
		{
			SHAPE_PROFILE_FUNCTION();
			if (!EnableValidationLayers)
				return;

			VkDebugReportCallbackCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			createInfo.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(DebugCallback);

			VkResult result = CreateDebugReportCallbackEXT(s_VkInstance, &createInfo, nullptr, &m_DebugCallback);
			if (result != VK_SUCCESS)
			{
				SHAPE_LOG_CRITICAL("[VULKAN] Failed to set up debug callback!");
			}
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

			//验证层支持
			if (!CheckValidationLayerSupport(m_InstanceLayerNames))
			{
				SHAPE_LOG_CRITICAL("[VULKAN] Validation layers requested, but not available!");
			}

			//扩展支持
			if (!CheckExtensionSupport(m_InstanceExtensionNames))
			{
				SHAPE_LOG_CRITICAL("[VULKAN] Extensions requested are not available!");
			}

			//创建实例
			VkApplicationInfo appInfo = {};

			uint32_t sdkVersion = VK_HEADER_VERSION_COMPLETE;
			uint32_t driverVersion = 0;

			// if enumerateInstanceVersion  is missing, only vulkan 1.0 supported
		   // https://www.lunarg.com/wp-content/uploads/2019/02/Vulkan-1.1-Compatibility-Statement_01_19.pdf
			auto enumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

			if (enumerateInstanceVersion)
			{
				enumerateInstanceVersion(&driverVersion);
			}
			else
			{
				driverVersion = VK_API_VERSION_1_0;
			}

			//选择支持的版本
			appInfo.apiVersion = Maths::Min(sdkVersion, driverVersion);

			m_VKVersion = appInfo.apiVersion;

			// SDK not supported
			if (sdkVersion > driverVersion)
			{
				// Detect and log version
				std::string driverVersionStr = StringUtilities::ToString(VK_API_VERSION_MAJOR(driverVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_MINOR(driverVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_PATCH(driverVersion));
				std::string sdkVersionStr = StringUtilities::ToString(VK_API_VERSION_MAJOR(sdkVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_MINOR(sdkVersion)) + "." + StringUtilities::ToString(VK_API_VERSION_PATCH(sdkVersion));
				SHAPE_LOG_WARN("Using Vulkan {0}. Please update your graphics drivers to support Vulkan {1}.", driverVersionStr, sdkVersionStr);
			}

			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Runtime";
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Shape";
			appInfo.engineVersion = VK_MAKE_VERSION(ShapeVersion.major, ShapeVersion.minor, ShapeVersion.patch);

			VkInstanceCreateInfo createInfo = {};
			createInfo.pApplicationInfo = &appInfo;
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(m_InstanceExtensionNames.size());
			createInfo.ppEnabledExtensionNames = m_InstanceExtensionNames.data();
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_InstanceLayerNames.size());
			createInfo.ppEnabledLayerNames = m_InstanceLayerNames.data();
			createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
			const std::vector<const char*> validation_layers = { "VK_LAYER_KHRONOS_validation" };

			const bool enableFeatureValidation = false;
			if (enableFeatureValidation)
			{
				std::vector<VkValidationFeatureEnableEXT> validation_extensions = {};
				validation_extensions.emplace_back(VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
				validation_extensions.emplace_back(VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);

				VkValidationFeaturesEXT validation_features = {};
				validation_features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
				validation_features.enabledValidationFeatureCount = static_cast<uint32_t>(validation_extensions.size());
				validation_features.pEnabledValidationFeatures = validation_extensions.data();
				createInfo.pNext = &validation_features;
			}

			VkResult createResult = vkCreateInstance(&createInfo, nullptr, &s_VkInstance);
			if (createResult != VK_SUCCESS)
			{
				SHAPE_LOG_CRITICAL("[VULKAN] Failed to create instance!");
			}
			volkLoadInstance(s_VkInstance);
		}

		void VKContext::MakeDefault()
		{
			CreateFunc = CreateFuncVulkan;
		}

		GraphicsContext* VKContext::CreateFuncVulkan()
		{
			return new VKContext();
		}

		void VKContext::WaitIdle() const
		{
			vkDeviceWaitIdle(VKDevice::Get().GetDevice());
		}
	}
}