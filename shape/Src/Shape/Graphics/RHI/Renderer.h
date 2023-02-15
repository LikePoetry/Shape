#pragma once
#include "Shape/Core/Application.h"
#include "Shape/Core/OS/Window.h"
#include "CommandBuffer.h"
#include "Pipeline.h"


namespace Shape
{
	namespace Graphics
	{
		class Mesh;

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

			static GraphicsContext* GetGraphicsContext() { return Application::Get().GetWindow()->GetGraphicsContext(); }
			static SwapChain* GetMainSwapChain() { return Application::Get().GetWindow()->GetSwapChain(); }
			static void DrawMesh(CommandBuffer* commandBuffer, Graphics::Pipeline* pipeline, Graphics::Mesh* mesh);

		protected:
			static Renderer* (*CreateFunc)();
			static Renderer* s_Instance;
		};
	}
}