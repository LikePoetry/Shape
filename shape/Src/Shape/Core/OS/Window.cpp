#include "hzpch.h"
#include "Window.h"

namespace Shape
{
    Window* (*Window::CreateFunc)(const WindowDesc&) = NULL;

    Window::~Window()
    {


    }

    Window* Window::Create(const WindowDesc& windowDesc)
    {
        SHAPE_ASSERT(CreateFunc, "No Windows Create Function");
        return CreateFunc(windowDesc);
    }

    bool Window::Initialise(const WindowDesc& windowDesc)
    {
        return HasInitialised();
    }
}