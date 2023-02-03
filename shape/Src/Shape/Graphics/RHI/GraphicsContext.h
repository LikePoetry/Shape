#pragma once
#include "Shape/Core/Core.h"


namespace Shape
{
	namespace Graphics
	{
		enum class RenderAPI :uint32_t
		{
			OPENGL = 0,
			VULKAN,
			DIRECT3D, // Unsupported
			METAL,    // Unsupported
			NONE,     // Unsupported
		};
	}

}