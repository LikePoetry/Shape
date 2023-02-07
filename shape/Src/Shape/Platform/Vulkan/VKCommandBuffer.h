#pragma once

#include "VK.h"
#include "Shape/Graphics/RHI/CommandBuffer.h"

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
			~VKCommandBuffer();

			bool Init(bool primary) override;

			static void MakeDefault();

		protected:
			static CommandBuffer* CreateFuncVulkan();
		};
	}
}