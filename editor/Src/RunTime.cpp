#include "Shape/Core/Application.h"
#include "Shape/Core/EntryPoint.h"
#include "Shape/Core/Profiler.h"

using namespace Shape;

class Runtime : public Application
{
	friend class Application;
public:
	explicit Runtime()
		: Application()
	{
		SHAPE_PROFILE_FUNCTION();
	}

	~Runtime()
	{
	}
};

Shape::Application* Shape::CreateApplication()
{
	return new ::Runtime();
}
