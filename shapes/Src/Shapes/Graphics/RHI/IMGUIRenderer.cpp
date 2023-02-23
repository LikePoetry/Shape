#include "Precompiled.h"
#include "IMGUIRenderer.h"
#include "GraphicsContext.h"

#ifdef SHAPES_IMGUI
#ifdef SHAPES_RENDER_API_OPENGL
#include "Platform/OpenGL/GLIMGUIRenderer.h"
#endif

#ifdef SHAPES_RENDER_API_VULKAN
#include "Platform/Vulkan/VKIMGUIRenderer.h"
#endif
#endif

namespace Shapes
{
    namespace Graphics
    {
        IMGUIRenderer* (*IMGUIRenderer::CreateFunc)(uint32_t, uint32_t, bool) = nullptr;

        IMGUIRenderer* IMGUIRenderer::Create(uint32_t width, uint32_t height, bool clearScreen)
        {
            SHAPES_ASSERT(CreateFunc, "No IMGUIRenderer Create Function");

            return CreateFunc(width, height, clearScreen);
        }
    }
}
