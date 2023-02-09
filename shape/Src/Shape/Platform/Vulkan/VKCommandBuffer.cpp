#include "hzpch.h"
#include "VKCommandBuffer.h"
#include "VKInitialisers.h"
#include "VKUtilities.h"

namespace Shape
{
    namespace Graphics
    {
        VKCommandBuffer::VKCommandBuffer()
            :m_CommandBuffer(nullptr)
            ,m_CommandPool(nullptr)
            ,m_Semaphore(nullptr)
            ,m_Primary(false)
            ,m_State(CommandBufferState::Idle)
        {

        }

        VKCommandBuffer::VKCommandBuffer(VkCommandBuffer commandBuffer)
            : m_CommandBuffer(commandBuffer)
            , m_CommandPool(nullptr)
            , m_Semaphore(nullptr)
            , m_Primary(true)
            , m_State(CommandBufferState::Idle)
        {
        }

        VKCommandBuffer::~VKCommandBuffer()
        {

        }

        bool VKCommandBuffer::Init(bool primary)
        {
            SHAPE_PROFILE_FUNCTION();
            m_Primary = primary;
            m_CommandPool = VKDevice::Get().GetCommandPool()->GetHandle();

            VkCommandBufferAllocateInfo cmdBufferCreateInfo = VKInitialisers::CommandBufferAllocateInfo(m_CommandPool, primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1);

            VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().GetDevice(), &cmdBufferCreateInfo, &m_CommandBuffer));

            VkSemaphoreCreateInfo semaphoreInfo = VKInitialisers::SemaphoreCreateInfo();
            semaphoreInfo.pNext = nullptr;

            VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().GetDevice(), &semaphoreInfo, nullptr, &m_Semaphore));
            m_Fence = CreateSharedPtr<VKFence>(false);

            return true;
        }

        bool VKCommandBuffer::Init(bool primary, VkCommandPool commandPool)
        {
            SHAPE_PROFILE_FUNCTION();
            m_Primary = primary;

            m_CommandPool = commandPool;

            VkCommandBufferAllocateInfo cmdBufferCreateInfo = VKInitialisers::CommandBufferAllocateInfo(m_CommandPool, primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY, 1);

            VK_CHECK_RESULT(vkAllocateCommandBuffers(VKDevice::Get().GetDevice(), &cmdBufferCreateInfo, &m_CommandBuffer));

            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            semaphoreInfo.pNext = nullptr;

            VK_CHECK_RESULT(vkCreateSemaphore(VKDevice::Get().GetDevice(), &semaphoreInfo, nullptr, &m_Semaphore));
            m_Fence = CreateSharedPtr<VKFence>(true);

            return true;
        }

        void VKCommandBuffer::Reset()
        {
            SHAPE_PROFILE_FUNCTION();
            VK_CHECK_RESULT(vkResetCommandBuffer(m_CommandBuffer, 0));
        }

        bool VKCommandBuffer::Wait()
        {
            SHAPE_PROFILE_FUNCTION();
            SHAPE_ASSERT(m_State == CommandBufferState::Submitted, "");

            m_Fence->WaitAndReset();
            m_State = CommandBufferState::Idle;

            return true;
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