#pragma once
#include "Shape/Graphics/RHI/GraphicsContext.h"
#include "VK.h"
#include "VKDevice.h"
#include "Shape/Core/Reference.h"

#include <deque>

#ifdef USE_VMA_ALLOCATOR
#include <vulkan/vk_mem_alloc.h>
#endif

const bool EnableValidationLayers = true;

namespace Shape
{
	namespace Graphics
	{
		class VKContext:public GraphicsContext
		{
		public:
			VKContext();
			~VKContext();

			static void MakeDefault();

			void Init() override;

			static VkInstance GetVKInstance() { return s_VkInstance; }
		protected:
			static GraphicsContext* CreateFuncVulkan();

			void CreateInstance();
			void SetupDebugCallback();
			bool CheckValidationLayerSupport(std::vector<const char*>& validationLayer);
			bool CheckExtensionSupport(std::vector<const char*>& extensions);

			static const std::vector<const char*> GetRequiredExtensions();
			const std::vector<const char*> GetRequiredLayers() const;

		private:
			static VkInstance s_VkInstance;
			VkDebugReportCallbackEXT m_DebugCallback = VK_NULL_HANDLE;

			std::vector<VkLayerProperties> m_InstanceLayers;
			std::vector<VkExtensionProperties> m_InstanceExtensions;

			std::vector<const char*> m_InstanceLayerNames;
			std::vector<const char*> m_InstanceExtensionNames;

			static uint32_t m_VKVersion;
		};
	}
}