#pragma once
#include "Core.h"

namespace Shape
{
	namespace Internal
	{
		class SHAPE_EXPORT CoreSystem
		{
		public:
			static bool Init();
			static void Shutdown();
		};
	}
}
