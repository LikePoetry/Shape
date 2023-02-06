#pragma once
#include "Shape/Core/Application.h"
#include "Shape/Core/OS/Window.h"

namespace Shape
{
	namespace Graphics
	{
		struct RenderAPICapabilities
		{
			std::string Vendor;
			std::string Renderer;
			std::string Version;

			int MaxSamples = 0;
			float MaxAnisotropy = 0.0f;
			int MaxTextureUnits = 0;
			int UniformBufferOffsetAlignment = 0;
			bool WideLines = false;
			bool SupportCompute = false;
		};

		class SHAPE_EXPORT Renderer
		{
		public:
			Renderer() = default;
			virtual ~Renderer() = default;

			static RenderAPICapabilities& GetCapabilities()
			{
				static RenderAPICapabilities capabilities;
				return capabilities;
			}
		};
	}
}