#pragma once
#include "Graphics/RHI/RenderDevice.h"

namespace Shape
{
	namespace Graphics
	{
		class VKRenderDevice:public RenderDevice
		{
		public:
			VKRenderDevice() = default;
			~VKRenderDevice() = default;

			void Init() override;

			static void MakeDefault();

		protected:
			static RenderDevice* CreateFuncVulkan();
		};
	}
}