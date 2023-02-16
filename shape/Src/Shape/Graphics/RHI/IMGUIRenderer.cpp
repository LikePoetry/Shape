#include "hzpch.h"
#include "IMGUIRenderer.h"
#include "GraphicsContext.h"

#include "Shape/Platform/Vulkan/VKIMGUIRenderer.h"


namespace Shape
{
    namespace Graphics
    {
        IMGUIRenderer* (*IMGUIRenderer::CreateFunc)(uint32_t, uint32_t, bool) = nullptr;

        IMGUIRenderer* IMGUIRenderer::Create(uint32_t width, uint32_t height, bool clearScreen)
        {
            SHAPE_ASSERT(CreateFunc, "No IMGUIRenderer Create Function");

            return CreateFunc(width, height, clearScreen);
        }
    }
}