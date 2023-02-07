#include "hzpch.h"
#include "SwapChain.h"

namespace Shape
{
	namespace Graphics
	{
		SwapChain* (*SwapChain::CreateFunc)(uint32_t, uint32_t) = nullptr;

		SwapChain* SwapChain::Create(uint32_t width,uint32_t height)
		{
			SHAPE_ASSERT(CreateFunc, "No SwapChain Create Function");

			return CreateFunc(width, height);
		}
	}
}