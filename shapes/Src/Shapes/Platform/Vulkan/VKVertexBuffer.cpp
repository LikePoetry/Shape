#include "Precompiled.h"
#include "VKDevice.h"
#include "VKVertexBuffer.h"
#include "VKRenderer.h"
#include "VKPipeline.h"

namespace Shapes
{
    namespace Graphics
    {
        VKVertexBuffer::VKVertexBuffer(const BufferUsage& usage)
            : VKBuffer()
            , m_Usage(usage)
            , m_Size(0)
        {
            VKBuffer::SetUsage(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
            VKBuffer::SetMemoryProperyFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        }

        VKVertexBuffer::~VKVertexBuffer()
        {
            SHAPES_PROFILE_FUNCTION();
            if(m_MappedBuffer)
            {
                VKBuffer::Flush(m_Size);
                VKBuffer::UnMap();
                m_MappedBuffer = false;
            }
        }

        void VKVertexBuffer::Resize(uint32_t size)
        {
            SHAPES_PROFILE_FUNCTION();

            if(m_Size != size)
            {
                m_Size = size;
                VKBuffer::Resize(size, nullptr);
            }
        }

        void VKVertexBuffer::SetData(uint32_t size, const void* data)
        {
            SHAPES_PROFILE_FUNCTION();
            if(m_Size != size)
            {
                m_Size = size;
                VKBuffer::Resize(size, data);
            }
            else
            {
                VKBuffer::SetData(size, data);
            }
        }

        void VKVertexBuffer::SetDataSub(uint32_t size, const void* data, uint32_t offset)
        {
            SHAPES_PROFILE_FUNCTION();
            m_Size = size;

            if(m_Size != size)
            {
                m_Size = size;
                VKBuffer::Resize(size, data);
            }
            else
            {
                VKBuffer::SetData(size, data);
            }
        }

        void* VKVertexBuffer::GetPointerInternal()
        {
            SHAPES_PROFILE_FUNCTION();
            if(!m_MappedBuffer)
            {
                VKBuffer::Map();
                m_MappedBuffer = true;
            }

            return m_Mapped;
        }

        void VKVertexBuffer::ReleasePointer()
        {
            SHAPES_PROFILE_FUNCTION();
            if(m_MappedBuffer)
            {
                VKBuffer::Flush(m_Size);
                VKBuffer::UnMap();
                m_MappedBuffer = false;
            }
        }

        void VKVertexBuffer::Bind(CommandBuffer* commandBuffer, Pipeline* pipeline)
        {
            SHAPES_PROFILE_FUNCTION();
            VkDeviceSize offsets[1] = { 0 };
            if(commandBuffer)
                vkCmdBindVertexBuffers(static_cast<VKCommandBuffer*>(commandBuffer)->GetHandle(), 0, 1, &m_Buffer, offsets);
        }

        void VKVertexBuffer::Unbind()
        {
        }

        void VKVertexBuffer::MakeDefault()
        {
            CreateFunc = CreateFuncVulkan;
        }

        VertexBuffer* VKVertexBuffer::CreateFuncVulkan(const BufferUsage& usage)
        {
            return new VKVertexBuffer(usage);
        }
    }
}
