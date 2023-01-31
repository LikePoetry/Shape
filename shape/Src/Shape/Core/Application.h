#pragma once
#include "Core.h"

namespace Shape
{
	class SHAPE_EXPORT Application
	{
		friend class RunTime;
	public:
		Application();
		virtual ~Application();

	};

	// Defined by client
	Application* CreateApplication();
}