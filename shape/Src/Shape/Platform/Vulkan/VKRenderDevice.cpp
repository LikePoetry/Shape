#include "hzpch.h"
#include "VKRenderDevice.h"

namespace Shape::Graphics
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