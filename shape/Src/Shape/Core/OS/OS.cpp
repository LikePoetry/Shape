#include "hzpch.h"
#include "OS.h"

#include "Shape/Platform/Windows/WindowsOS.h"

namespace Shape
{
	OS* OS::s_Instance = nullptr;

	void OS::Create()
	{
		SHAPE_ASSERT(!s_Instance, "OS already exists!");

		s_Instance = new WindowsOS();
	}

	void OS::Release()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

    std::string OS::PowerStateToString(PowerState state)
    {
        switch (state)
        {
        case POWERSTATE_UNKNOWN:
            return std::string("Unknown");
            break;
        case POWERSTATE_NO_BATTERY:
            return std::string("No Battery");
            break;
        case POWERSTATE_CHARGED:
            return std::string("Charged");
            break;
        case POWERSTATE_CHARGING:
            return std::string("Charging");
            break;
        case POWERSTATE_ON_BATTERY:
            return std::string("On Battery");
            break;
        default:
            return std::string("Error");
            break;
        }
    }
}