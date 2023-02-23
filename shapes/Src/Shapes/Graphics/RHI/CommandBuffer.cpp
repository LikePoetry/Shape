#include "Precompiled.h"
#include "CommandBuffer.h"

namespace Shapes
{
    namespace Graphics
    {
        CommandBuffer* (*CommandBuffer::CreateFunc)() = nullptr;

        CommandBuffer* CommandBuffer::Create()
        {
            SHAPES_ASSERT(CreateFunc, "No CommandBuffer Create Function");

            return CreateFunc();
        }
    }
}
