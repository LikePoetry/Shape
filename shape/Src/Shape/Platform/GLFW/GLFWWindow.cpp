#include "hzpch.h"
#include "GLFWWindow.h"

#include "Shape/Graphics/RHI/GraphicsContext.h"

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
		SHAPE_PROFILE_FUNCTION();
		m_Init = false;
		m_VSync = properties.VSync;

		SHAPE_LOG_INFO("VSync : {0}", m_VSync ? "True" : "False");
		m_HasResized = true;
		m_Data.m_RenderAPI = static_cast<Graphics::RenderAPI>(properties.RenderAPI);
		m_Data.VSync = m_VSync;
		m_Init = Init(properties);

		//初始化图形工具和交换链
	}

	GLFWWindow::~GLFWWindow()
	{

	}

	/// <summary>
	/// 创建GLFWwindow
	/// </summary>
	/// <param name="properties"></param>
	/// <returns></returns>
	bool GLFWWindow::Init(const WindowDesc& properties)
	{
		SHAPE_PROFILE_FUNCTION();
		SHAPE_LOG_INFO("Creating window - Title : {0}, Width : {1}, Height : {2}", properties.Title, properties.Width, properties.Height);
		
		if(!s_GLFWInitialized)
		{
			int success = glfwInit();
			SHAPE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}
		s_NumGLFWWindows++;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		float xscale, yscale;
		glfwGetMonitorContentScale(monitor, &xscale, &yscale);
		m_Data.DPIScale = xscale;

		SetBorderlessWindow(properties.Borderless);
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;

		if (properties.Fullscreen)
		{
			ScreenWidth = mode->width;
			ScreenHeight = mode->height;
		}
		else
		{
			ScreenWidth = properties.Width;
			ScreenHeight = properties.Height;
		}

		m_Data.Title = properties.Title;
		m_Data.Width = ScreenWidth;
		m_Data.Height = ScreenHeight;
		m_Data.Exit = false;

		//使用VULKAN 初始化
		if (m_Data.m_RenderAPI == Graphics::RenderAPI::VULKAN)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Handle = glfwCreateWindow(ScreenWidth, ScreenHeight, properties.Title.c_str(), nullptr, nullptr);
		int w, h;
		glfwGetFramebufferSize(m_Handle, &w, &h);
		m_Data.Width = w;
		m_Data.Height = h;

		glfwSetWindowUserPointer(m_Handle, &m_Data);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(m_Handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		//设定应用图标
		//SetIcon("", "");

		glfwSetInputMode(m_Handle, GLFW_STICKY_KEYS, true);

		//TODO Set GLFW callbacks


		g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

		SHAPE_LOG_INFO("Initialised GLFW version : {0}", glfwGetVersionString());

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