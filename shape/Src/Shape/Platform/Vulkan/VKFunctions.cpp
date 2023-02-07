#include "hzpch.h"
#include "VKFunctions.h"

#include "VKContext.h"
#include "VKSwapChain.h"

void Shape::Graphics::Vulkan::MakeDefault()
{
	VKContext::MakeDefault();
	VKSwapChain::MakeDefault();
}