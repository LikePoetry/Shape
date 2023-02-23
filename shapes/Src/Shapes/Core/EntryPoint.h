#if defined(SHAPES_PLATFORM_WINDOWS)

#include "Core/CoreSystem.h"
#include "Platform/Windows/WindowsOS.h"

#pragma comment(linker, "/subsystem:windows")

#ifndef NOMINMAX
#define NOMINMAX // For windows.h
#endif

#include <windows.h>

extern Shapes::Application* Shapes::CreateApplication();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifndef SHAPES_PRODUCTION
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    if(!Shapes::Internal::CoreSystem::Init(0, nullptr))
        return 0;

    auto windowsOS = new Shapes::WindowsOS();
    Shapes::OS::SetInstance(windowsOS);

    windowsOS->Init();

    Shapes::CreateApplication();

    windowsOS->Run();
    delete windowsOS;

    Shapes::Internal::CoreSystem::Shutdown();
    return 0;
}

#elif defined(SHAPES_PLATFORM_LINUX)

#include "Core/CoreSystem.h"
#include "Platform/Unix/UnixOS.h"

extern Shapes::Application* Shapes::CreateApplication();

int main(int argc, char** argv)
{
    if(!Shapes::Internal::CoreSystem::Init(argc, argv))
        return 0;

    auto unixOS = new Shapes::UnixOS();
    Shapes::OS::SetInstance(unixOS);
    unixOS->Init();

    Shapes::CreateApplication();

    unixOS->Run();
    delete unixOS;

    Shapes::Internal::CoreSystem::Shutdown();
}

#elif defined(SHAPES_PLATFORM_MACOS)

#include "Platform/MacOS/MacOSOS.h"

int main(int argc, char** argv)
{
    if(!Shapes::Internal::CoreSystem::Init(argc, argv))
        return 0;

    auto macOSOS = new Shapes::MacOSOS();
    Shapes::OS::SetInstance(macOSOS);
    macOSOS->Init();

    Shapes::CreateApplication();

    macOSOS->Run();
    delete macOSOS;

    Shapes::Internal::CoreSystem::Shutdown();
}

#elif defined(SHAPES_PLATFORM_IOS)

#endif
