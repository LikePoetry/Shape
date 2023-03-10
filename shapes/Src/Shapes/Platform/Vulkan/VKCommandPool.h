#pragma once

#include "VK.h"

namespace Shapes
{
    namespace Graphics
    {
        class VKCommandPool
        {
        public:
            VKCommandPool(int queueIndex, VkCommandPoolCreateFlags flags);
            ~VKCommandPool();

            void Reset();

            const VkCommandPool& GetHandle() const { return m_Handle; }

        private:
            VkCommandPool m_Handle;
        };
    }
}
