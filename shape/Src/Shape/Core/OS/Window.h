#pragma once
#include "Events/Event.h"
#include "Graphics/RHI/GraphicsContext.h"
#include "Graphics/RHI/SwapChain.h"
#include <glm/vec2.hpp>
#include <glm/fwd.hpp>

namespace Shape
{
	struct SHAPE_EXPORT WindowDesc
	{
        WindowDesc(uint32_t width = 1280, uint32_t height = 720, int renderAPI = 0, const std::string& title = "Shape", bool fullscreen = false, bool vSync = true, bool borderless = false, const std::string& filepath = "")
            : Width(width)
            , Height(height)
            , Title(title)
            , Fullscreen(fullscreen)
            , VSync(vSync)
            , Borderless(borderless)
            , RenderAPI(renderAPI)
            , FilePath(filepath)
        {
        }

        uint32_t Width, Height;
        bool Fullscreen;
        bool VSync;
        bool Borderless;
        bool ShowConsole = true;
        std::string Title;
        int RenderAPI;
        std::string FilePath;
	};

    class SHAPE_EXPORT Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        static Window* Create(const WindowDesc& windowDesc);
        virtual ~Window();
        bool Initialise(const WindowDesc& windowDesc);

        bool HasInitialised() const
        {
            return m_Init;
        };

        virtual bool GetExit() const = 0;
        virtual void SetExit(bool exit) = 0;

        inline void SetHasResized(bool resized)
        {
            m_HasResized = resized;
        }
        inline bool GetHasResized() const
        {
            return m_HasResized;
        }

        virtual void ToggleVSync() = 0;
        virtual void SetVSync(bool set) = 0;
        virtual void SetWindowTitle(const std::string& title) = 0;
        virtual void OnUpdate() = 0;
        virtual void ProcessInput() {};
        virtual void SetBorderlessWindow(bool borderless) = 0;
        virtual void* GetHandle()
        {
            return nullptr;
        };
        virtual float GetScreenRatio() const = 0;
        virtual void HideMouse(bool hide) {};
        virtual void SetMousePosition(const glm::vec2& pos) {};
        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
        virtual void UpdateCursorImGui() = 0;
        virtual void SetIcon(const std::string& filePath, const std::string& smallIconFilePath = "") = 0;
        virtual void Maximise() {};
        virtual std::string GetTitle() const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual float GetDPIScale() const { return 1.0f; }
        virtual bool GetVSync() const { return m_VSync; };

        void SetWindowFocus(bool focus) { m_WindowFocus = focus; }
        bool GetWindowFocus() const { return m_WindowFocus; }

        const SharedPtr<Shape::Graphics::SwapChain>& GetSwapChain() const { return m_SwapChain; }
        const SharedPtr<Shape::Graphics::GraphicsContext>& GetGraphicsContext() const { return m_GraphicsContext; }

    protected:
        static Window* (*CreateFunc)(const WindowDesc&);

        Window() = default;

        bool m_Init = false;
        glm::vec2 m_Position;
        bool m_VSync = false;
        bool m_HasResized = false;
        bool m_WindowFocus = true;

        SharedPtr<Shape::Graphics::GraphicsContext> m_GraphicsContext;
        SharedPtr<Shape::Graphics::SwapChain> m_SwapChain;
    };
}