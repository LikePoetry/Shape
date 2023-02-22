#pragma once
#include "VK.h"
#include "VKContext.h"
#include "VKSwapChain.h"
#include "Graphics/RHI/DescriptorSet.h"
#include "Graphics/RHI/RenderPass.h"
#include "VKUniformBuffer.h"
#include "VKDescriptorSet.h"
#include "Graphics/RHI/Renderer.h"

#define NUM_SEMAPHORES 10

namespace Shape
{
	namespace Graphics
	{
		class CommandBuffer;

		class SHAPE_EXPORT VKRenderer :public Renderer
		{
		public:
			VKRenderer() = default;
			~VKRenderer();

			static VKRenderer* GetRenderer()
			{
				return static_cast<VKRenderer*>(s_Instance);
			}

			static VKContext* GetGraphicsContext()
			{
				return static_cast<VKContext*>(s_Instance->GetGraphicsContext());
			}

			static VKSwapChain* GetMainSwapChain()
			{
				return static_cast<VKSwapChain*>(Renderer::GetMainSwapChain());
			}

			void InitInternal() override;
			void Begin() override;
			void OnResize(uint32_t width, uint32_t height) override;

			void PresentInternal() override;
			void PresentInternal(CommandBuffer* commandBuffer) override;

			void ClearRenderTarget(Graphics::Texture* texture, Graphics::CommandBuffer* commandBuffer, glm::vec4 clearColour) override;
			void ClearSwapChainImage() const;

			const std::string& GetTitleInternal() const override;

			void BindDescriptorSetsInternal(Graphics::Pipeline* pipeline, Graphics::CommandBuffer* commandBuffer, uint32_t dynamicOffset, Graphics::DescriptorSet** descriptorSets, uint32_t descriptorCount) override;
			void DrawIndexedInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start) const override;
			void DrawInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, DataType datayType, void* indices) const override;
			void DrawSplashScreen(Texture* texture) override;
			uint32_t GetGPUCount() const override;
			bool SupportsCompute() override { return true; }
			void Dispatch(CommandBuffer* commandBuffer, uint32_t workGroupSizeX, uint32_t workGroupSizeY, uint32_t workGroupSizeZ) override;

			static VkDescriptorPool& GetDescriptorPool()
			{
				return s_DescriptorPool;
			};

			static VKContext::DeletionQueue& GetDeletionQueue(int frameIndex)
			{
				SHAPE_ASSERT(frameIndex < int(s_DeletionQueue.size()), "Unsupported Frame Index");
				return s_DeletionQueue[frameIndex];
			}

			static VKContext::DeletionQueue& GetCurrentDeletionQueue()
			{
				return s_DeletionQueue[s_DeletionQueueIndex];
			}

			static void FlushDeletionQueues()
			{
				for (auto& deletionQueue : s_DeletionQueue)
					deletionQueue.Flush();
			}

			template <typename F>
			static void PushDeletionFunction(F&& function)
			{
				GetCurrentDeletionQueue().PushFunction(function);
			}

			static void MakeDefault();

		protected:
			static Renderer* CreateFuncVulkan();

		private:
			uint32_t m_CurrentSemaphoreIndex = 0;

			std::string m_RendererTitle;
			uint32_t m_DescriptorCapacity = 0;

			VkDescriptorSet m_DescriptorSetPool[16] = {};

			static VkDescriptorPool s_DescriptorPool;
			static std::vector<VKContext::DeletionQueue> s_DeletionQueue;
			static int s_DeletionQueueIndex;

		};
	}
}