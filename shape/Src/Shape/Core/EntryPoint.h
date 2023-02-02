#pragma once
#include "CoreSystem.h"
/// Windows ϵͳ

extern Shape::Application* Shape::CreateApplication();

int main()
{
	if (!Shape::Internal::CoreSystem::Init())
		return 0;

	//实例化Window

	Shape::CreateApplication();

	Shape::Internal::CoreSystem::Shutdown();
	return 0;
}