#pragma once
#include "Reference.h"

namespace Shape
{
	class Window;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;

	enum class AppState
	{
		Running,
		Loading,
		Closing
	};

	class SHAPE_EXPORT Application
	{
		friend class RunTime;
	public:
		Application();
		virtual ~Application();

		void Run();
		bool OnFrame();

		virtual void Init();
		virtual void OnEvent(Event& e);

		Window* GetWindow() const { return m_Window.get(); }

		static void Release()
		{
			if (s_Instance)
				delete s_Instance;
			s_Instance = nullptr;
		}

		static Application& Get() { return *s_Instance; }

		bool OnWindowResize(WindowResizeEvent& e);

		//œÓƒø…Ë÷√
		struct ProjectSettings
		{
			std::string m_ProjectRoot;
			std::string m_ProjectName;
			std::string m_EngineAssetPath;
			uint32_t Width = 1200, Height = 800;
			bool Fullscreen = true;
			bool VSync = true;
			bool Borderless = false;
			bool ShowConsole = true;
			std::string Title;
			int RenderAPI;
			int ProjectVersion;
			int8_t DesiredGPUIndex = -1;
		};

		ProjectSettings& GetProjectSettings() { return m_ProjectSettings; }

	protected:
		ProjectSettings m_ProjectSettings;
		bool m_ProjectLoaded = false;
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		UniquePtr<Window> m_Window;

		AppState m_CurrentState = AppState::Loading;

		static Application* s_Instance;

	};

	// Defined by client
	Application* CreateApplication();
}