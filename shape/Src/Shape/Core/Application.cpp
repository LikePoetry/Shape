#include "hzpch.h"
#include "Application.h"
#include "OS/Window.h"
#include "Shape/Events/ApplicationEvent.h"

namespace Shape
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		SHAPE_PROFILE_FUNCTION();
		SHAPE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
	}

	Application::~Application()
	{

	}

	void Application::Init()
	{
		WindowDesc windowDesc;
		windowDesc.Width = m_ProjectSettings.Width;
		windowDesc.Height = m_ProjectSettings.Height;
		windowDesc.RenderAPI = m_ProjectSettings.RenderAPI;
		windowDesc.Fullscreen = m_ProjectSettings.Fullscreen;
		windowDesc.Borderless = m_ProjectSettings.Borderless;
		windowDesc.ShowConsole = m_ProjectSettings.ShowConsole;
		windowDesc.Title = m_ProjectSettings.Title;
		windowDesc.VSync = m_ProjectSettings.VSync;
		
		// 初始化窗口
		m_Window = UniquePtr<Window>(Window::Create(windowDesc));
	}

	void Application::Run()
	{
		while (true)
		{

		}
		//退出循环主体
	}


}