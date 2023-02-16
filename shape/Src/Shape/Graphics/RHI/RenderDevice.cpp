#include "hzpch.h"
#include "RenderDevice.h"

namespace Shape
{
    namespace Graphics
    {
        RenderDevice* (*RenderDevice::CreateFunc)() = nullptr;

        RenderDevice* RenderDevice::s_Instance = nullptr;

        void RenderDevice::Create()
        {
            SHAPE_ASSERT(CreateFunc, "No RenderDevice Create Function");

            s_Instance = CreateFunc();
        }

        void RenderDevice::Release()
        {
            delete s_Instance;
        };
    }
}