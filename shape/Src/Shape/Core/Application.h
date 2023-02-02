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

		static Application& Get() { return *s_Instance; }

	private:
		static Application* s_Instance;

	};

	// Defined by client
	Application* CreateApplication();
}