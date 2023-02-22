#include "hzpch.h"
#include "WindowsOS.h"

#include "Core/Application.h"

#include "Platform/GLFW/GLFWWindow.h"

#include <Windows.h>
#include <filesystem>

extern Shape::Application* Shape::CreateApplication();

namespace Shape
{
	void WindowsOS::Run()
	{
		auto& app = Shape::Application::Get();
		app.Init();
		app.Run();
		app.Release();
	}

	void WindowsOS::Init()
	{
		// Use GLFWWindow
		GLFWWindow::MakeDefault();
	}

	std::string WindowsOS::GetExecutablePath()
	{
		WCHAR path[MAX_PATH];
		GetModuleFileNameW(NULL, path, MAX_PATH);

		// To fix warnings
		// std::wstring ws(path);
		std::string convertedString = std::filesystem::path(path).string();
		; // std::string(ws.begin(), ws.end());
		std::replace(convertedString.begin(), convertedString.end(), '\\', '/');

		return convertedString;
	}
}
