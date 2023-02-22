#include "hzpch.h"
#include "GraphicsContext.h"

#include "Platform/Vulkan/VKContext.h"
#include "Platform/Vulkan/VKFunctions.h"

namespace Shape
{
	namespace Graphics
	{
		GraphicsContext* (*GraphicsContext::CreateFunc)() = nullptr;

		RenderAPI GraphicsContext::s_RenderAPI;

		GraphicsContext* GraphicsContext::Create()
		{
			SHAPE_ASSERT(CreateFunc, "No GraphicsContext Create Function");
			return CreateFunc();
		}

		GraphicsContext::~GraphicsContext()
		{
		}

		void GraphicsContext::SetRenderAPI(RenderAPI api)
		{
			s_RenderAPI = api;

			switch (s_RenderAPI)
			{
			case RenderAPI::VULKAN:
				Graphics::Vulkan::MakeDefault();
				break;
			default:
				break;
			}
		}
	}
}