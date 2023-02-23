#include "Precompiled.h"
#include "VertexBuffer.h"

namespace Shapes
{
    namespace Graphics
    {
        VertexBuffer* (*VertexBuffer::CreateFunc)(const BufferUsage&) = nullptr;

        VertexBuffer* VertexBuffer::Create(const BufferUsage& usage)
        {
            SHAPES_ASSERT(CreateFunc, "No VertexBuffer Create Function");

            return CreateFunc(usage);
        }
    }
}
