#include "hzpch.h"
#include "CommandBuffer.h"

namespace Shape
{
	namespace Graphics
	{
		CommandBuffer* (*CommandBuffer::CreateFunc)() = nullptr;

		CommandBuffer* CommandBuffer::Create()
		{
			SHAPE_ASSERT(CreateFunc, "No CommandBuffer Create Function");
			return CreateFunc();
		}
	}
}