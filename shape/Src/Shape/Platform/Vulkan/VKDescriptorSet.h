#pragma once
#include "Shape/Graphics/RHI/DescriptorSet.h"
#include "VK.h"
#include "Shape/Graphics/RHI/Renderer.h"
#include "Shape/Graphics/RHI/SwapChain.h"
#include "Shape/Core/Buffer.h"

#define MAX_BUFFER_INFOS 32
#define MAX_IMAGE_INFOS 32
#define MAX_WRITE_DESCRIPTORS 32

namespace Shape
{
	namespace Graphics
	{
		class VKDescriptorSet:public DescriptorSet
		{
		public:
			VKDescriptorSet(const DescriptorDesc& descriptorDesc);
			~VKDescriptorSet();

			VkDescriptorSet GetDescriptorSet()
			{
				uint32_t currentFrame = Renderer::GetMainSwapChain()->GetCurrentBufferIndex();
				return m_DescriptorSet[currentFrame];
			}
		};
	}
}