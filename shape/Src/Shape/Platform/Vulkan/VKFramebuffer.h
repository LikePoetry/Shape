#pragma once
#include "Shape/Core/Core.h"
#include "Shape/Graphics/RHI/Framebuffer.h"
#include "VK.h"
#include "VKTexture.h"
#include "VKRenderPass.h"

namespace Shape
{
	namespace Graphics
	{
		class SHAPE_EXPORT VKFramebuffer :public Framebuffer
		{
		public:
			VKFramebuffer(const FramebufferDesc& frameBufferInfo);
			~VKFramebuffer();

			const VkFramebuffer& GetFramebuffer() const { return m_Framebuffer; }


		protected:
			static Framebuffer* CreateFuncVulkan(const FramebufferDesc&);

		private:
			uint32_t m_Width;
			uint32_t m_Height;
			uint32_t m_AttachmentCount;
			VkFramebuffer m_Framebuffer;
		};
	}
}