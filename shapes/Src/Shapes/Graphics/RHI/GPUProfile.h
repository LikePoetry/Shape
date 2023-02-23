#pragma once
#include "Graphics/RHI/GraphicsContext.h"

#if defined(SHAPES_RENDER_API_VULKAN)
#include "Platform/Vulkan/VKDevice.h"
#include "Platform/Vulkan/VKCommandBuffer.h"
#endif

#if defined(SHAPES_PROFILE_GPU_ENABLED) && defined(SHAPES_RENDER_API_VULKAN) && defined(SHAPES_PROFILE) && defined(TRACY_ENABLE)
#define SHAPES_PROFILE_GPU(name) TracyVkZone(Shapes::Graphics::VKDevice::Get().GetTracyContext(), static_cast<Shapes::Graphics::VKCommandBuffer*>(Renderer::GetMainSwapChain()->GetCurrentCommandBuffer())->GetHandle(), name)
#else
#define SHAPES_PROFILE_GPU(name)
#endif
