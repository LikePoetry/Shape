#include "hzpch.h"
#include "VertexBuffer.h"

namespace Shape
{
	namespace Graphics
	{
		VertexBuffer* (*VertexBuffer::CreateFunc)(const BufferUsage&) = nullptr;

		VertexBuffer* VertexBuffer::Create(const BufferUsage& usage)
		{
			SHAPE_ASSERT(CreateFunc, "No VertexBuffer Create Function");

			return CreateFunc(usage);
		}
	}
}