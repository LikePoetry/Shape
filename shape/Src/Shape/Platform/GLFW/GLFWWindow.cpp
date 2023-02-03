#include "hzpch.h"
#include "GLFWWindow.h"

#include "GLFWKeyCodes.h"

#include "Shape/Core/OS/OS.h"
#include "Shape/Core/OS/Input.h"
#include "Shape/Core/Application.h"

#include "Shape/Events/ApplicationEvent.h"
#include "Shape/Events/MouseEvent.h"
#include "Shape/Events/KeyEvent.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui/imgui.h>


static GLFWcursor* g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

namespace Shape
{
	static bool s_GLFWInitialized = false;
	static int s_NumGLFWWindows = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		SHAPE_LOG_ERROR("GLFW Error - {0} : {1}", error, description);
	}

	GLFWWindow::GLFWWindow(const WindowDesc& properties)
	{

	}

	GLFWWindow::~GLFWWindow()
	{

	}

	bool GLFWWindow::Init(const WindowDesc& properties)
	{
		return true;
	}

	void GLFWWindow::SetIcon(const std::string& file, const std::string& smallIconFilePath)
	{

	}

	void GLFWWindow::SetWindowTitle(const std::string& title)
	{

	}

	void GLFWWindow::ToggleVSync()
	{

	}

	void GLFWWindow::SetVSync(bool set)
	{

	}

	void GLFWWindow::OnUpdate()
	{

	}

	void GLFWWindow::SetBorderlessWindow(bool borderless)
	{

	}

	void GLFWWindow::HideMouse(bool hide)
	{

	}

	void GLFWWindow::SetMousePosition(const glm::vec2& pos)
	{

	}

	void GLFWWindow::MakeDefault()
	{
		CreateFunc = CreateFuncGLFW;
	}

	Window* GLFWWindow::CreateFuncGLFW(const WindowDesc& properties)
	{
		return new GLFWWindow(properties);
	}

	void GLFWWindow::UpdateCursorImGui()
	{

	}

	void GLFWWindow::ProcessInput()
	{

	}

	void GLFWWindow::Maximise()
	{

	}
}