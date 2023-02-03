#pragma once
#include "Reference.h"

namespace Shape
{
	class Window;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;


	class SHAPE_EXPORT Application
	{
		friend class RunTime;
	public:
		Application();
		virtual ~Application();

		void Run();

		virtual void Init();

		static void Release()
		{
			if (s_Instance)
				delete s_Instance;
			s_Instance = nullptr;
		}

		static Application& Get() { return *s_Instance; }

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
		UniquePtr<Window> m_Window;

		static Application* s_Instance;

	};

	// Defined by client
	Application* CreateApplication();
}