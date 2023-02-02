#pragma once
#include "CoreSystem.h"
/// Windows ϵͳ
#include "Shape/Platform/Windows/WindowsOS.h"

extern Shape::Application* Shape::CreateApplication();

int main()
{
	if (!Shape::Internal::CoreSystem::Init())
		return 0;

	//实例化Window
	auto windowsOS = new Shape::WindowsOS();
	Shape::OS::SetInstance(windowsOS);

	windowsOS->Init();

	Shape::CreateApplication();

	windowsOS->Run();
	delete windowsOS;

	Shape::Internal::CoreSystem::Shutdown();
	return 0;
}