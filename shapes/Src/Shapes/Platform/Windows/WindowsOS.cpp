#include "Precompiled.h"
#include "WindowsOS.h"
#include "WindowsPower.h"
#include "WindowsWindow.h"
#include "Core/CoreSystem.h"
#include "Core/OS/MemoryManager.h"
#include "Core/Application.h"

#ifdef SHAPES_USE_GLFW_WINDOWS
#include "Platform/GLFW/GLFWWindow.h"
#endif

#include <Windows.h>
#include <filesystem>

extern Shapes::Application* Shapes::CreateApplication();

namespace Shapes
{
    void WindowsOS::Run()
    {
        auto power       = WindowsPower();
        auto percentage  = power.GetPowerPercentageLeft();
        auto secondsLeft = power.GetPowerSecondsLeft();
        auto state       = power.GetPowerState();

        SHAPES_LOG_INFO("--------------------");
        SHAPES_LOG_INFO(" System Information ");
        SHAPES_LOG_INFO("--------------------");

        if(state != PowerState::POWERSTATE_NO_BATTERY)
            SHAPES_LOG_INFO("Battery Info - Percentage : {0} , Time Left {1}s , State : {2}", percentage, secondsLeft, PowerStateToString(state));
        else
            SHAPES_LOG_INFO("Power - Outlet");

        auto systemInfo = MemoryManager::Get()->GetSystemInfo();
        systemInfo.Log();

        auto& app = Shapes::Application::Get();
        app.Init();
        app.Run();
        app.Release();
    }

    void WindowsOS::Init()
    {
#ifdef SHAPES_USE_GLFW_WINDOWS
        GLFWWindow::MakeDefault();
#else
        WindowsWindow::MakeDefault();
#endif
    }

    SystemMemoryInfo MemoryManager::GetSystemInfo()
    {
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&status);

        SystemMemoryInfo result = {
            (int64_t)status.ullAvailPhys,
            (int64_t)status.ullTotalPhys,

            (int64_t)status.ullAvailVirtual,
            (int64_t)status.ullTotalVirtual
        };
        return result;
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
