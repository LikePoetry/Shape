#pragma once
#include "VKCommandBuffer.h"
#include "VKContext.h"
#include "Shape/Graphics/RHI/SwapChain.h"
#include "Shape/Graphics/RHI/Texture.h"

#define MAX_SWAPCHAIN_BUFFERS 3

namespace Shape
{
    class Window;
    namespace Graphics
    {
        class VKFence;

        struct FrameData
        {
            VkSemaphore PresentSemaphore = VK_NULL_HANDLE;
            SharedPtr<VKCommandPool> CommandPool;
            SharedPtr<VKCommandBuffer> MainCommandBuffer;
        };

        class VKSwapChain:public SwapChain
        {
        public:
            VKSwapChain(uint32_t width, uint32_t height);
            ~VKSwapChain();

            bool Init(bool vsync) override;
            bool Init(bool vsync, Window* windowHandle) override;
            void CreateFrameData();

            VkSurfaceKHR CreatePlatformSurface(VkInstance vkInstance, Window* window);

            static void MakeDefault();

        protected:
            static SwapChain* CreateFuncVulkan(uint32_t width, uint32_t height);

        
        private:
            FrameData m_Frames[MAX_SWAPCHAIN_BUFFERS];
            void FindImageFormatAndColourSpace();

            std::vector<Texture2D*> m_SwapChainBuffers;

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