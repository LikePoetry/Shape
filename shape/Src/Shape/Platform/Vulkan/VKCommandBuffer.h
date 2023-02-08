#pragma once

#include "VK.h"
#include "Shape/Graphics/RHI/CommandBuffer.h"
#include "VKFence.h"
#include "VKDevice.h"

namespace Shape
{
	namespace Graphics
	{
		enum class CommandBufferState :uint8_t
		{
			Idle,
			Recording,
			Ended,
			Submitted
		};

		class VKCommandBuffer:public CommandBuffer
		{
		public:
			VKCommandBuffer();
			VKCommandBuffer(VkCommandBuffer commandBuffer);
			~VKCommandBuffer();

			bool Init(bool primary) override;

			bool Wait();
			void Reset();

			CommandBufferState GetState() const { return m_State; }

			VkSemaphore GetSemaphore() const { return m_Semaphore; }

			static void MakeDefault();

		protected:
			static CommandBuffer* CreateFuncVulkan();

		private:
			VkCommandBuffer m_CommandBuffer;
			VkCommandPool m_CommandPool;
			bool m_Primary;
			CommandBufferState m_State;
			SharedPtr<VKFence> m_Fence;
			VkSemaphore m_Semaphore;

		};
	}
}