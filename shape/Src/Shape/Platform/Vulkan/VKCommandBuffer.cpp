#include "hzpch.h"
#include "VKCommandBuffer.h"

namespace Shape
{
    namespace Graphics
    {
        VKCommandBuffer::VKCommandBuffer()
        {

        }

        VKCommandBuffer::~VKCommandBuffer()
        {

        }

        bool VKCommandBuffer::Init(bool primary)
        {

        }

        void VKCommandBuffer::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
        }

        CommandBuffer* VKCommandBuffer::CreateFuncVulkan()
        {
            return new VKCommandBuffer();
        }
    }
}