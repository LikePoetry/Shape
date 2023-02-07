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
		m_GraphicsContext = SharedPtr<Graphics::GraphicsContext>(Graphics::GraphicsContext::Create());
		m_GraphicsContext->Init();

		m_SwapChain = SharedPtr<Graphics::SwapChain>(Graphics::SwapChain::Create(m_Data.Width, m_Data.Height));
		m_SwapChain->Init(m_VSync, (Window*)this);
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

		if (!s_GLFWInitialized)
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

		//Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);

		data.DPIScale = (float)w / (float)width;

		data.Width = uint32_t(width * data.DPIScale);
		data.Height = uint32_t(height * data.DPIScale);

		WindowResizeEvent event(data.Width, data.Height, data.DPIScale);
		data.EventCallback(event); });

		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));
		WindowCloseEvent event;
		data.EventCallback(event);
		data.Exit = true; });

		glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* window, int focused)
			{
				Window* lmWindow = Application::Get().GetWindow();

		if (lmWindow)
			lmWindow->SetWindowFocus(focused); });

		glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* window, int32_t state)
			{
				switch (state)
				{
				case GL_TRUE:
					Application::Get().GetWindow()->SetWindowFocus(false);
					break;
				case GL_FALSE:
					Application::Get().GetWindow()->SetWindowFocus(true);
					break;
				default:
					SHAPE_LOG_INFO("Unsupported window iconify state from callback");
				} });

		glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

		switch (action)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(GLFWKeyCodes::GLFWToShapeKeyboardKey(key), 0);
			data.EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(GLFWKeyCodes::GLFWToShapeKeyboardKey(key));
			data.EventCallback(event);
			break;
		}
		case GLFW_REPEAT:
		{
			KeyPressedEvent event(GLFWKeyCodes::GLFWToShapeKeyboardKey(key), 1);
			data.EventCallback(event);
			break;
		}
		} });

		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

		switch (action)
		{
		case GLFW_PRESS:
		{
			MouseButtonPressedEvent event(GLFWKeyCodes::GLFWToShapeMouseKey(button));
			data.EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			MouseButtonReleasedEvent event(GLFWKeyCodes::GLFWToShapeMouseKey(button));
			data.EventCallback(event);
			break;
		}
		} });

		glfwSetScrollCallback(m_Handle, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));
		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.EventCallback(event); });

		glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));
		MouseMovedEvent event((float)xPos /* * data.DPIScale*/, (float)yPos /* * data.DPIScale*/);
		data.EventCallback(event); });

		glfwSetCursorEnterCallback(m_Handle, [](GLFWwindow* window, int enter)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

		MouseEnterEvent event(enter > 0);
		data.EventCallback(event); });

		glfwSetCharCallback(m_Handle, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

		KeyTypedEvent event(GLFWKeyCodes::GLFWToShapeKeyboardKey(keycode), char(keycode));
		data.EventCallback(event); });

		glfwSetDropCallback(m_Handle, [](GLFWwindow* window, int numDropped, const char** filenames)
			{
				WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

		std::string filePath = filenames[0];
		WindowFileEvent event(filePath);
		data.EventCallback(event); });

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
		SHAPE_PROFILE_SCOPE("GLFW PollEvents");
		glfwPollEvents();
	}

	void GLFWWindow::Maximise()
	{

	}
}