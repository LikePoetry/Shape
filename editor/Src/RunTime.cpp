#include "Shape/Core/Application.h"
#include "Shape/Core/EntryPoint.h"

using namespace Shape;

class Runtime : public Application
{
	friend class Application;
public:
	explicit Runtime()
		: Application()
	{
	}

	~Runtime()
	{
	}
};

Shape::Application* Shape::CreateApplication()
{
	return new ::Runtime();
}
