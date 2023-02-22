#include "hzpch.h"
#include "Platform/Vulkan/VKSwapChain.h"
#include "Platform/GLFW/GLFWWindow.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan_win32.h>

namespace Shape
{
    VkSurfaceKHR Graphics::VKSwapChain::CreatePlatformSurface(VkInstance vkInstance, Window* window)
    {
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(vkInstance, static_cast<GLFWwindow*>(window->GetHandle()), nullptr, (VkSurfaceKHR*)&surface);

        return surface;
    }

    static const char* GetPlatformSurfaceExtension()
    {
        return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
    }
}