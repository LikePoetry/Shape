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

			std::string ErrorString(VkResult errorCode);
		}
	}
}