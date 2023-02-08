#include "hzpch.h"
#include "VKUtilities.h"
namespace Shape
{
    namespace Graphics
    {
        std::string VKUtilities::ErrorString(VkResult errorCode)
        {
            switch (errorCode)
            {
#define STR(r)   \
    case VK_##r: \
        return #r
                STR(NOT_READY);
                STR(TIMEOUT);
                STR(EVENT_SET);
                STR(EVENT_RESET);
                STR(INCOMPLETE);
                STR(ERROR_OUT_OF_HOST_MEMORY);
                STR(ERROR_OUT_OF_DEVICE_MEMORY);
                STR(ERROR_INITIALIZATION_FAILED);
                STR(ERROR_DEVICE_LOST);
                STR(ERROR_MEMORY_MAP_FAILED);
                STR(ERROR_LAYER_NOT_PRESENT);
                STR(ERROR_EXTENSION_NOT_PRESENT);
                STR(ERROR_FEATURE_NOT_PRESENT);
                STR(ERROR_INCOMPATIBLE_DRIVER);
                STR(ERROR_TOO_MANY_OBJECTS);
                STR(ERROR_FORMAT_NOT_SUPPORTED);
                STR(ERROR_SURFACE_LOST_KHR);
                STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
                STR(SUBOPTIMAL_KHR);
                STR(ERROR_OUT_OF_DATE_KHR);
                STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
                STR(ERROR_VALIDATION_FAILED_EXT);
                STR(ERROR_INVALID_SHADER_NV);
                STR(SUCCESS);
                STR(ERROR_FRAGMENTED_POOL);
                STR(ERROR_OUT_OF_POOL_MEMORY);
                STR(ERROR_INVALID_EXTERNAL_HANDLE);
                STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
                STR(ERROR_FRAGMENTATION_EXT);
                STR(ERROR_NOT_PERMITTED_EXT);
                STR(ERROR_INVALID_DEVICE_ADDRESS_EXT);
                STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
                STR(ERROR_UNKNOWN);
                STR(RESULT_MAX_ENUM);
#undef STR
            default:
                return "UNKNOWN_ERROR";
            }
        }

        bool VKUtilities::IsPresentModeSupported(const std::vector<VkPresentModeKHR>& supportedModes, VkPresentModeKHR presentMode)
        {
            for (const auto& mode : supportedModes)
            {
                if (mode == presentMode)
                {
                    return true;
                }
            }
            return false;
        }

        VkPresentModeKHR VKUtilities::ChoosePresentMode(const std::vector<VkPresentModeKHR>& supportedModes, bool vsync)
        {
            VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
            if (!vsync)
            {
                if(IsPresentModeSupported(supportedModes,VK_PRESENT_MODE_IMMEDIATE_KHR))
                {
                    presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                }
                else
                {
                    SHAPE_LOG_ERROR("Failed to find supported presentation mode.");
                }
            }
            return presentMode;
        }

        VkFormat VKUtilities::FormatToVK(const RHIFormat format, bool srgb)
        {
            if (srgb)
            {
                switch (format)
                {
                case RHIFormat::R8_Unorm:
                    return VK_FORMAT_R8_SRGB;
                case RHIFormat::R8G8_Unorm:
                    return VK_FORMAT_R8G8_SRGB;
                case RHIFormat::R8G8B8_Unorm:
                    return VK_FORMAT_R8G8B8_SRGB;
                case RHIFormat::R8G8B8A8_Unorm:
                    return VK_FORMAT_R8G8B8A8_SRGB;
                case RHIFormat::R16G16B16_Float:
                    return VK_FORMAT_R16G16B16_SFLOAT;
                case RHIFormat::R16G16B16A16_Float:
                    return VK_FORMAT_R16G16B16A16_SFLOAT;
                case RHIFormat::R32G32B32_Float:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case RHIFormat::R32G32B32A32_Float:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                default:
                    SHAPE_LOG_CRITICAL("[Texture] Unsupported image bit-depth!");
                    return VK_FORMAT_R8G8B8A8_SRGB;
                }
            }
            else
            {
                switch (format)
                {
                case RHIFormat::R8_Unorm:
                    return VK_FORMAT_R8_UNORM;
                case RHIFormat::R8G8_Unorm:
                    return VK_FORMAT_R8G8_UNORM;
                case RHIFormat::R8G8B8_Unorm:
                    return VK_FORMAT_R8G8B8A8_UNORM;
                case RHIFormat::R8G8B8A8_Unorm:
                    return VK_FORMAT_R8G8B8A8_UNORM;
                case RHIFormat::R11G11B10_Float:
                    return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
                case RHIFormat::R10G10B10A2_Unorm:
                    return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
                case RHIFormat::R16_Float:
                    return VK_FORMAT_R16_SFLOAT;
                case RHIFormat::R16G16_Float:
                    return VK_FORMAT_R16G16_SFLOAT;
                case RHIFormat::R16G16B16_Float:
                    return VK_FORMAT_R16G16B16_SFLOAT;
                case RHIFormat::R16G16B16A16_Float:
                    return VK_FORMAT_R16G16B16A16_SFLOAT;
                case RHIFormat::R32_Float:
                    return VK_FORMAT_R32_SFLOAT;
                case RHIFormat::R32G32_Float:
                    return VK_FORMAT_R32G32_SFLOAT;
                case RHIFormat::R32G32B32_Float:
                    return VK_FORMAT_R32G32B32_SFLOAT;
                case RHIFormat::R32G32B32A32_Float:
                    return VK_FORMAT_R32G32B32A32_SFLOAT;
                case RHIFormat::D16_Unorm:
                    return VK_FORMAT_D16_UNORM;
                case RHIFormat::D32_Float:
                    return VK_FORMAT_D32_SFLOAT;
                case RHIFormat::D24_Unorm_S8_UInt:
                    return VK_FORMAT_D24_UNORM_S8_UINT;
                case RHIFormat::D32_Float_S8_UInt:
                    return VK_FORMAT_D32_SFLOAT_S8_UINT;
                default:
                    SHAPE_LOG_CRITICAL("[Texture] Unsupported image bit-depth!");
                    return VK_FORMAT_R8G8B8A8_UNORM;
                }
            }
        }
    }
}