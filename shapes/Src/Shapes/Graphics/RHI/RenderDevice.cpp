#include "Precompiled.h"
#include "RenderDevice.h"

namespace Shapes
{
    namespace Graphics
    {
        RenderDevice* (*RenderDevice::CreateFunc)() = nullptr;

        RenderDevice* RenderDevice::s_Instance = nullptr;

        void RenderDevice::Create()
        {
            SHAPES_ASSERT(CreateFunc, "No RenderDevice Create Function");

            s_Instance = CreateFunc();
        }

        void RenderDevice::Release()
        {
            delete s_Instance;
        };
    }
}
