#pragma once
#include "VKContext.h"
#include "Shape/Graphics/RHI/SwapChain.h"

#define MAX_SWAPCHAIN_BUFFERS 3

namespace Shape
{
    class Window;
    namespace Graphics
    {
        class VKSwapChain:public SwapChain
        {
        public:
            VKSwapChain(uint32_t width, uint32_t height);
            ~VKSwapChain();

            bool Init(bool vsync) override;
            bool Init(bool vsync, Window* windowHandle) override;

            VkSurfaceKHR CreatePlatformSurface(VkInstance vkInstance, Window* window);

            static void MakeDefault();

        protected:
            static SwapChain* CreateFuncVulkan(uint32_t width, uint32_t height);

        
        private:

            void FindImageFormatAndColourSpace();

            uint32_t m_CurrentBuffer = 0;
            uint32_t m_AcquireImageIndex = 0;
            uint32_t m_Width;
            uint32_t m_Height;
            uint32_t m_QueueNodeIndex = UINT32_MAX;
            uint32_t m_SwapChainBufferCount;
            bool m_VSyncEnabled = false;

            VkSwapchainKHR m_SwapChain;
            VkSwapchainKHR m_OldSwapChain;
            VkSurfaceKHR m_Surface;
            VkFormat m_ColourFormat;
            VkColorSpaceKHR m_ColourSpace;
        };
    }
}