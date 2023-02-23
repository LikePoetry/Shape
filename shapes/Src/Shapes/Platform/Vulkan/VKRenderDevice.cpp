#include "Precompiled.h"
#include "VKRenderDevice.h"

namespace Shapes::Graphics
{
    void VKRenderDevice::Init()
    {
    }

    void VKRenderDevice::MakeDefault()
    {
        CreateFunc = CreateFuncVulkan;
    }

    RenderDevice* VKRenderDevice::CreateFuncVulkan()
    {
        return new VKRenderDevice();
    }
}
