#pragma once
#include "VKCommandBuffer.h"
#include "VKContext.h"
#include "Graphics/RHI/SwapChain.h"
#include "Graphics/RHI/Texture.h"

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
            void AcquireNextImage();
            void QueueSubmit();
            void Present(VkSemaphore semaphore);
            void Begin();
            void End();
            void OnResize(uint32_t width, uint32_t height, bool forceResize = false, Window* windowHandle = nullptr);

            VkSurfaceKHR GetSurface() const { return m_Surface; }
            VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
            uint32_t GetCurrentImageIndex() const override { return m_AcquireImageIndex; }

            uint32_t GetCurrentBufferIndex() const override { return m_CurrentBuffer; }
            size_t GetSwapChainBufferCount() const override { return m_SwapChainBufferCount; };
            Texture* GetCurrentImage() override { return (Texture*)m_SwapChainBuffers[m_AcquireImageIndex]; };
            Texture* GetImage(uint32_t index) override { return (Texture*)m_SwapChainBuffers[index]; };

            VkSurfaceKHR CreatePlatformSurface(VkInstance vkInstance, Window* window);

            CommandBuffer* GetCurrentCommandBuffer() override;
            void SetVSync(bool vsync) override { m_VSyncEnabled = vsync; }

            FrameData& GetCurrentFrameData();
            VkFormat GetScreenFormat() const { return m_ColourFormat; }

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