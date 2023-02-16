#pragma once
#include "Shape/Graphics/RHI/GraphicsContext.h"

#include "Shape/Platform/Vulkan/VKDevice.h"
#include "Shape/Platform/Vulkan/VKCommandBuffer.h"

#if defined(TRACY_ENABLE)
#define SHAPE_PROFILE_GPU(name) TracyVkZone(Shape::Graphics::VKDevice::Get().GetTracyContext(), static_cast<Shape::Graphics::VKCommandBuffer*>(Renderer::GetMainSwapChain()->GetCurrentCommandBuffer())->GetHandle(), name)
#endif