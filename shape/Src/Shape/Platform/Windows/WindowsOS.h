#pragma once
#include "Core/OS/OS.h"

namespace Shape
{
	class SHAPE_EXPORT WindowsOS:public OS
	{
	public:
		WindowsOS() = default;
		~WindowsOS() = default;

		void Init();
		void Run() override;
		std::string GetExecutablePath() override;
	};
}