#include "hzpch.h"
#include "UniformBuffer.h"

namespace Shape
{
	namespace Graphics
	{
		UniformBuffer* (*UniformBuffer::CreateFunc)();
		UniformBuffer* (*UniformBuffer::CreateDataFunc)(uint32_t, const void*) = nullptr;

		UniformBuffer* UniformBuffer::Create()
		{
			SHAPE_ASSERT(CreateFunc, "No UniformBuffer Create Function");

			return CreateFunc();
		}

		UniformBuffer* UniformBuffer::Create(uint32_t size, const void* data)
		{
			SHAPE_ASSERT(CreateFunc, "No UniformBuffer Create Function");

			return CreateDataFunc(size, data);
		}
	}
}