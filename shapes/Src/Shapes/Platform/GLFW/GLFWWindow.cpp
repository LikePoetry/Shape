#include "Precompiled.h"

#if defined(SHAPES_PLATFORM_MACOS)
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include "GLFWWindow.h"
#include "Graphics/RHI/GraphicsContext.h"
#include "Graphics/RHI/SwapChain.h"

#include "Utilities/LoadImage.h"

#include "GLFWKeyCodes.h"

#include "Core/OS/OS.h"
#include "Core/OS/Input.h"
#include "Core/Application.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui/imgui.h>

static GLFWcursor* g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

namespace Shapes
{
    static bool s_GLFWInitialized = false;
    static int s_NumGLFWWindows   = 0;

    static void GLFWErrorCallback(int error, const char* description)
    {
        SHAPES_LOG_ERROR("GLFW Error - {0} : {1}", error, description);
    }

    GLFWWindow::GLFWWindow(const WindowDesc& properties)
    {
        SHAPES_PROFILE_FUNCTION();
        m_Init  = false;
        m_VSync = properties.VSync;

        SHAPES_LOG_INFO("VSync : {0}", m_VSync ? "True" : "False");
        m_HasResized       = true;
        m_Data.m_RenderAPI = static_cast<Graphics::RenderAPI>(properties.RenderAPI);
        m_Data.VSync       = m_VSync;
        m_Init             = Init(properties);

        // Setting fullscreen overrides width and heigh in Init
        auto propCopy   = properties;
        propCopy.Width  = m_Data.Width;
        propCopy.Height = m_Data.Height;

        m_GraphicsContext = SharedPtr<Graphics::GraphicsContext>(Graphics::GraphicsContext::Create());
        m_GraphicsContext->Init();

        m_SwapChain = SharedPtr<Graphics::SwapChain>(Graphics::SwapChain::Create(m_Data.Width, m_Data.Height));
        m_SwapChain->Init(m_VSync, (Window*)this);
    }

    GLFWWindow::~GLFWWindow()
    {
        for(ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
        {
            glfwDestroyCursor(g_MouseCursors[cursor_n]);
            g_MouseCursors[cursor_n] = NULL;
        }

        glfwDestroyWindow(m_Handle);

        s_NumGLFWWindows--;

        if(s_NumGLFWWindows < 1)
        {
            s_GLFWInitialized = false;
            glfwTerminate();
        }
    }

    bool GLFWWindow::Init(const WindowDesc& properties)
    {
        SHAPES_PROFILE_FUNCTION();
        SHAPES_LOG_INFO("Creating window - Title : {0}, Width : {1}, Height : {2}", properties.Title, properties.Width, properties.Height);

        if(!s_GLFWInitialized)
        {
            int success = glfwInit();
            SHAPES_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);

            s_GLFWInitialized = true;
        }

        s_NumGLFWWindows++;

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        float xscale, yscale;
        glfwGetMonitorContentScale(monitor, &xscale, &yscale);
        m_Data.DPIScale = xscale;

#ifdef SHAPES_PLATFORM_MACOS
        if(m_Data.DPIScale > 1.0f)
        {
            glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
            glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
        }
        else
        {
            glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
            glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
        }
#endif

#ifdef SHAPES_RENDER_API_OPENGL
        if(m_Data.m_RenderAPI == Graphics::RenderAPI::OPENGL)
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef SHAPES_PLATFORM_MACOS
            glfwWindowHint(GLFW_SAMPLES, 1);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
            glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GL_TRUE);
            glfwWindowHint(GLFW_STENCIL_BITS, 8); // Fixes 16 bit stencil bits in macOS.
            glfwWindowHint(GLFW_STEREO, GLFW_FALSE);
            glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
#endif
        }
#endif
        SetBorderlessWindow(properties.Borderless);

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        uint32_t ScreenWidth  = 0;
        uint32_t ScreenHeight = 0;

        if(properties.Fullscreen)
        {
            ScreenWidth  = mode->width;
            ScreenHeight = mode->height;
        }
        else
        {
            ScreenWidth  = properties.Width;
            ScreenHeight = properties.Height;
        }

        m_Data.Title  = properties.Title;
        m_Data.Width  = ScreenWidth;
        m_Data.Height = ScreenHeight;
        m_Data.Exit   = false;

#ifdef SHAPES_RENDER_API_VULKAN
        if(m_Data.m_RenderAPI == Graphics::RenderAPI::VULKAN)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

        m_Handle = glfwCreateWindow(ScreenWidth, ScreenHeight, properties.Title.c_str(), nullptr, nullptr);

        int w, h;
        glfwGetFramebufferSize(m_Handle, &w, &h);
        m_Data.Width  = w;
        m_Data.Height = h;

#ifdef SHAPES_RENDER_API_OPENGL
        if(m_Data.m_RenderAPI == Graphics::RenderAPI::OPENGL)
        {
            glfwMakeContextCurrent(m_Handle);

            if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            {
                SHAPES_LOG_ERROR("Failed to initialise OpenGL context");
            }
        }
#endif

        glfwSetWindowUserPointer(m_Handle, &m_Data);

        if(glfwRawMouseMotionSupported())
            glfwSetInputMode(m_Handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

#ifdef SHAPES_PLATFORM_WINDOWS
        SetIcon("//Textures/icon.png", "//Textures/icon32.png");
#elif SHAPES_PLATFORM_LINUX
        SetIcon("//Textures/icon.png", "//Textures/icon32.png");
#endif
        // glfwSetWindowPos(m_Handle, mode->width / 2 - m_Data.Width / 2, mode->height / 2 - m_Data.Height / 2);
        glfwSetInputMode(m_Handle, GLFW_STICKY_KEYS, true);

        //#ifdef SHAPES_PLATFORM_WINDOWS
        //		glfwGetWindowSize(m_Handle, &w, &h);
        //		m_Data.DPIScale = (float)w / m_Data.Width;
        //#endif

        // Set GLFW callbacks
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

			if(lmWindow)
				lmWindow->SetWindowFocus(focused); });

        glfwSetWindowIconifyCallback(m_Handle, [](GLFWwindow* window, int32_t state)
                                     {
                switch(state)
                {
                case GL_TRUE:
                    Application::Get().GetWindow()->SetWindowFocus(false);
                    break;
                case GL_FALSE:
                    Application::Get().GetWindow()->SetWindowFocus(true);
                    break;
                default:
                    SHAPES_LOG_INFO("Unsupported window iconify state from callback");
                } });

        glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
                           {
                WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

                switch(action)
                {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(GLFWKeyCodes::GLFWToShapesKeyboardKey(key), 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(GLFWKeyCodes::GLFWToShapesKeyboardKey(key));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(GLFWKeyCodes::GLFWToShapesKeyboardKey(key), 1);
                    data.EventCallback(event);
                    break;
                }
                } });

        glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int button, int action, int mods)
                                   {
                WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

                switch(action)
                {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(GLFWKeyCodes::GLFWToShapesMouseKey(button));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(GLFWKeyCodes::GLFWToShapesMouseKey(button));
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

                KeyTypedEvent event(GLFWKeyCodes::GLFWToShapesKeyboardKey(keycode), char(keycode));
                data.EventCallback(event); });

        glfwSetDropCallback(m_Handle, [](GLFWwindow* window, int numDropped, const char** filenames)
                            {
            WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

            std::string filePath = filenames[0];
            WindowFileEvent event(filePath);
            data.EventCallback(event); });

        g_MouseCursors[ImGuiMouseCursor_Arrow]      = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        g_MouseCursors[ImGuiMouseCursor_TextInput]  = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        g_MouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        g_MouseCursors[ImGuiMouseCursor_ResizeNS]   = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        g_MouseCursors[ImGuiMouseCursor_ResizeEW]   = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        g_MouseCursors[ImGuiMouseCursor_Hand]       = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

        SHAPES_LOG_INFO("Initialised GLFW version : {0}", glfwGetVersionString());
        return true;
    }

    void GLFWWindow::SetIcon(const std::string& file, const std::string& smallIconFilePath)
    {
        uint32_t width, height;
        uint8_t* pixels = Shapes::LoadImageFromFile(file, &width, &height, nullptr, nullptr, true);

        if(!pixels)
        {
            SHAPES_LOG_WARN("Failed to load app icon {0}", file);
            return;
        }

        std::vector<GLFWimage> images;
        GLFWimage image;
        image.height = height;
        image.width  = width;
        image.pixels = static_cast<unsigned char*>(pixels);
        images.push_back(image);

        if(smallIconFilePath != "")
        {
            pixels = Shapes::LoadImageFromFile(smallIconFilePath, &width, &height, nullptr, nullptr, true);

            if(!pixels)
            {
                SHAPES_LOG_WARN("Failed to load app icon {0}", smallIconFilePath);
                return;
            }

            image.height = height;
            image.width  = width;
            image.pixels = static_cast<unsigned char*>(pixels);
            images.push_back(image);
        }

        glfwSetWindowIcon(m_Handle, int(images.size()), images.data());

        delete[] pixels;
    }

    void GLFWWindow::SetWindowTitle(const std::string& title)
    {
        SHAPES_PROFILE_FUNCTION();
        glfwSetWindowTitle(m_Handle, title.c_str());
    }

    void GLFWWindow::ToggleVSync()
    {
        SHAPES_PROFILE_FUNCTION();
        if(m_VSync)
        {
            SetVSync(false);
        }
        else
        {
            SetVSync(true);
        }

        SHAPES_LOG_INFO("VSync : {0}", m_VSync ? "True" : "False");
    }

    void GLFWWindow::SetVSync(bool set)
    {
        SHAPES_PROFILE_FUNCTION();

        m_VSync = set;
        if(Graphics::GraphicsContext::GetRenderAPI() == Graphics::RenderAPI::OPENGL)
            glfwSwapInterval(set ? 1 : 0);

        SHAPES_LOG_INFO("VSync : {0}", m_VSync ? "True" : "False");
    }

    void GLFWWindow::OnUpdate()
    {
        SHAPES_PROFILE_FUNCTION();
#ifdef SHAPES_RENDER_API_OPENGL
        if(m_Data.m_RenderAPI == Graphics::RenderAPI::OPENGL)
        {
            SHAPES_PROFILE_SCOPE("GLFW SwapBuffers");
            glfwSwapBuffers(m_Handle);
        }
#endif
    }

    void GLFWWindow::SetBorderlessWindow(bool borderless)
    {
        SHAPES_PROFILE_FUNCTION();
        if(borderless)
        {
            glfwWindowHint(GLFW_DECORATED, false);
        }
        else
        {
            glfwWindowHint(GLFW_DECORATED, true);
        }
    }

    void GLFWWindow::HideMouse(bool hide)
    {
        SHAPES_PROFILE_FUNCTION();
        if(hide)
        {
            glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else
        {
            glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    void GLFWWindow::SetMousePosition(const glm::vec2& pos)
    {
        SHAPES_PROFILE_FUNCTION();
        Input::Get().StoreMousePosition(pos.x, pos.y);
        glfwSetCursorPos(m_Handle, pos.x, pos.y);
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
        SHAPES_PROFILE_FUNCTION();
        ImGuiIO& io                   = ImGui::GetIO();
        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();

        if((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(m_Handle, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            return;

        if(imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor

            // TODO: This was disabled as it would override control of hiding the cursor
            //       Need to find a solution to support both
            // glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else
        {
            glfwSetCursor(m_Handle, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
            // glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    //#define LOG_CONTROLLER 0

    void GLFWWindow::ProcessInput()
    {
        SHAPES_PROFILE_SCOPE("GLFW PollEvents");
        glfwPollEvents();

        auto& controllers = Input::Get().GetControllers();
        for(auto it = controllers.begin(); it != controllers.end();)
        {
            int id = it->first;
            if(glfwJoystickPresent(id) != GLFW_TRUE)
                Input::Get().RemoveController(id);

            it++;
        }

        // Update controllers
        for(int id = GLFW_JOYSTICK_1; id < GLFW_JOYSTICK_LAST; id++)
        {
            if(glfwJoystickPresent(id) == GLFW_TRUE)
            {
#if LOG_CONTROLLER
                SHAPES_LOG_INFO("Controller Connected {0}", id);
#endif
                Controller* controller = Input::Get().GetOrAddController(id);
                if(controller)
                {
                    controller->ID   = id;
                    controller->Name = glfwGetJoystickName(id);

#if LOG_CONTROLLER
                    SHAPES_LOG_INFO(controller->Name);
#endif
                    int buttonCount;
                    const unsigned char* buttons = glfwGetJoystickButtons(id, &buttonCount);
                    for(int i = 0; i < buttonCount; i++)
                    {
                        controller->ButtonStates[i] = buttons[i] == GLFW_PRESS;

#if LOG_CONTROLLER
                        if(controller->ButtonStates[i])
                            SHAPES_LOG_INFO("Button pressed {0}", buttons[i]);
#endif
                    }

                    int axisCount;
                    const float* axes = glfwGetJoystickAxes(id, &axisCount);
                    for(int i = 0; i < axisCount; i++)
                    {
                        controller->AxisStates[i] = axes[i];
#if LOG_CONTROLLER
                        SHAPES_LOG_INFO(controller->AxisStates[i]);
#endif
                    }

                    int hatCount;
                    const unsigned char* hats = glfwGetJoystickHats(id, &hatCount);
                    for(int i = 0; i < hatCount; i++)
                    {
                        controller->HatStates[i] = hats[i];
#if LOG_CONTROLLER
                        SHAPES_LOG_INFO(controller->HatStates[i]);
#endif
                    }
                }
            }
        }
    }

    void GLFWWindow::Maximise()
    {
        SHAPES_PROFILE_FUNCTION();
        glfwMaximizeWindow(m_Handle);

#ifdef SHAPES_PLATFORM_MACOS
        // TODO: Move to glfw extensions or something
        OS::Instance()->MaximiseWindow();
#endif
    }
}
