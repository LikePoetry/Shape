#include "hzpch.h"
#include "Application.h"
#include "OS/Window.h"
#include "Shape/Events/ApplicationEvent.h"
#include "OS/FileSystem.h"
#include "Shape/Graphics/RHI/Renderer.h"

namespace Shape
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		SHAPE_PROFILE_FUNCTION();
		SHAPE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
	}

	Application::~Application()
	{

	}

	/// <summary>
	/// �˳�������!
	/// </summary>
	void Application::OnQuit()
	{

	}

	/// <summary>
	/// ��Ŀ��ʼ��
	/// </summary>
	void Application::Init()
	{
		SHAPE_PROFILE_FUNCTION();



		Graphics::GraphicsContext::SetRenderAPI(static_cast<Graphics::RenderAPI>(m_ProjectSettings.RenderAPI));

		WindowDesc windowDesc;
		windowDesc.Width = m_ProjectSettings.Width;
		windowDesc.Height = m_ProjectSettings.Height;
		windowDesc.RenderAPI = m_ProjectSettings.RenderAPI;
		windowDesc.Fullscreen = m_ProjectSettings.Fullscreen;
		windowDesc.Borderless = m_ProjectSettings.Borderless;
		windowDesc.ShowConsole = m_ProjectSettings.ShowConsole;
		windowDesc.Title = m_ProjectSettings.Title;
		windowDesc.VSync = m_ProjectSettings.VSync;

		// ��ʼ������
		m_Window = UniquePtr<Window>(Window::Create(windowDesc));
		if (!m_Window->HasInitialised())
			OnQuit();


		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_EditorState = EditorState::Play;

		m_ShaderLibrary = CreateSharedPtr<ShaderLibrary>();
		//m_ModelLibrary = CreateSharedPtr<ModelLibrary>();
		//m_FontLibrary = CreateSharedPtr<FontLibrary>();

		// ����Ƕ��ʽShader
		bool loadEmbeddedShaders = true;
		if (FileSystem::FolderExists(m_ProjectSettings.m_EngineAssetPath + "Shaders"))
			loadEmbeddedShaders = false;

		Graphics::Renderer::Init(loadEmbeddedShaders);
		

		m_CurrentState = AppState::Running;
	}

	void Application::Run()
	{
		while (OnFrame())
		{

		}
		//�˳�ѭ������
	}

	//��ѭ����
	bool Application::OnFrame()
	{
		m_Window->ProcessInput();

		return m_CurrentState != AppState::Closing;
	}

	/// <summary>
	/// �����¼��ص�����
	/// </summary>
	/// <param name="e"></param>
	void Application::OnEvent(Event& e)
	{
		SHAPE_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		//Imgui�¼�����
		//�������¼���Ӧ

		//������Ӧ
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_CurrentState = AppState::Closing;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		return false;
	}
}