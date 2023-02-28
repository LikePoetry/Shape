#include "Editor.h"

#include "Shapes/Core/OS/FileSystem.h"
#include "Shapes/Core/OS/OS.h"
#include "Shapes/Core/StringUtilities.h"
#include "Shapes/Core/OS/Input.h"
#include "Shapes/ImGui/ImGuiUtilities.h"
#include "Shapes/Scene/Scene.h"
#include "Shapes/Scene/Entity.h"
#include "Shapes/Scene/Component/ModelComponent.h"
#include "Shapes/Physics/B2PhysicsEngine/B2PhysicsEngine.h"
#include "Shapes/Physics/ShapesPhysicsEngine/ShapesPhysicsEngine.h"



#include "SceneViewPanel.h"

#include <Shapes/Graphics/Renderers/DebugRenderer.h>



#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/Plugins/ImGuizmo.h>

namespace Shapes
{
	Editor* Editor::s_Editor = nullptr;

	Editor::Editor()
	{
		s_Editor = this;
	}

	Editor::~Editor()
	{
	}

	void Editor::Init()
	{
		Application::Init();
		Application::SetEditorState(EditorState::Preview);
		Application::Get().GetWindow()->SetEventCallback(BIND_EVENT_FN(Editor::OnEvent));

		m_EditorCamera = CreateSharedPtr<Camera>(-20.0f,
			-40.0f,
			glm::vec3(-31.0f, 12.0f, 51.0f),
			60.0f,
			m_Settings.m_CameraNear,
			m_Settings.m_CameraFar,
			(float)Application::Get().GetWindowSize().x / (float)Application::Get().GetWindowSize().y);
		m_CurrentCamera = m_EditorCamera.get();

		glm::mat4 viewMat = glm::inverse(glm::lookAt(glm::vec3(-31.0f, 12.0f, 51.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		m_EditorCameraTransform.SetLocalTransform(viewMat);

		// 添加视图页面
		m_Panels.emplace_back(CreateSharedPtr<SceneViewPanel>());

		for (auto& panel : m_Panels)
			panel->SetEditor(this);

		// 创建网格线绘制类，用于后续网格线的绘制
		CreateGridRenderer();

		// 不显示 Imgui Demo window!
		m_Settings.m_ShowImGuiDemo = false;

		m_SelectedEntity = entt::null;
		m_PreviewTexture = nullptr;

		Application::Get().GetSystem<ShapesPhysicsEngine>()->SetDebugDrawFlags(m_Settings.m_Physics3DDebugFlags);
		Application::Get().GetSystem<B2PhysicsEngine>()->SetDebugDrawFlags(m_Settings.m_Physics2DDebugFlags);

		ImGuiUtilities::SetTheme(m_Settings.m_Theme);
		OS::Instance()->SetTitleBarColour(ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]);
		Application::Get().GetWindow()->SetWindowTitle("Lumos Editor");

		ImGuizmo::SetGizmoSizeClipSpace(m_Settings.m_ImGuizmoScale);
		// ImGuizmo::SetGizmoSizeScale(Application::Get().GetWindowDPI());
	}

	void Editor::OnImGui()
	{
		//非全屏模式
		BeginDockSpace(false);


		ImGui::Begin("Metrics");
		ImGui::Text("FPS : %.2f", (float)Shapes::Engine::Get().Statistics().FramesPerSecond);
		ImGui::End();

		for (auto& panel : m_Panels)
			panel->OnImGui();

		bool showDemo = true;
		ImGui::ShowDemoWindow(&showDemo);


		if (Application::Get().GetEditorState() == EditorState::Preview)
			Application::Get().GetSceneManager()->GetCurrentScene()->UpdateSceneGraph();

		EndDockSpace();

		Application::OnImGui();
	}

	void Editor::OnRender()
	{
		Application::OnRender();

		if (m_Settings.m_ShowGrid)
		{
			//绘制 3D 空间中的网格线
			Draw3DGrid();
		}
	}

	void Editor::OnEvent(Event& e)
	{
		SHAPES_PROFILE_FUNCTION();
		//EventDispatcher dispatcher(e);
		//dispatcher.Dispatch<WindowFileEvent>(BIND_EVENT_FN(Editor::OnFileDrop));
		// Block events here

		Application::OnEvent(e);
	}

	void Editor::OnUpdate(const TimeStep& ts)
	{
		// 处理输入事件
		SHAPES_PROFILE_FUNCTION();


		if(m_SceneViewActive)
		{
			auto& registry = Application::Get().GetSceneManager()->GetCurrentScene()->GetRegistry();

			{
				// 场景中操作鼠标，场景视角发生改变
				const glm::vec2 mousePos = Input::Get().GetMousePosition();

				m_EditorCameraController.HandleMouse(m_EditorCameraTransform, (float)ts.GetSeconds(), mousePos.x, mousePos.y);
				m_EditorCameraController.HandleKeyboard(m_EditorCameraTransform, (float)ts.GetSeconds());

			}
		}
		else 
		{
			m_EditorCameraController.StopMovement();
		}

		Application::OnUpdate(ts);
	}

	void Editor::OnNewScene(Scene* scene)
	{
		SHAPES_PROFILE_FUNCTION();
		Application::OnNewScene(scene);

		for (auto panel : m_Panels)
		{
			panel->OnNewScene(scene);
		}

		// TODO 测试代码，在场景中加入一个CUBE
		auto scene1 = Application::Get().GetSceneManager()->GetCurrentScene();
		auto entity = scene1->CreateEntity("Cube");
		entity.AddComponent<Graphics::ModelComponent>(Graphics::PrimitiveType::Cube);
	}


	//页面绘制相关
	//菜单栏绘制
	void Editor::DrawMenuBar()
	{

	}

	//开始绘制Dockspace
	void Editor::BeginDockSpace(bool gameFullScreen)
	{
		SHAPES_PROFILE_FUNCTION();

		static bool p_open = true;
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			auto pos = viewport->Pos;
			auto size = viewport->Size;
			bool menuBar = true;
			if (menuBar)
			{
				const float infoBarSize = ImGui::GetFrameHeight();
				pos.y += infoBarSize;
				size.y -= infoBarSize;
			}

			ImGui::SetNextWindowPos(pos);
			ImGui::SetNextWindowSize(size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
				| ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the
		// pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_DockSpace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("MyDockspace", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiID DockspaceID = ImGui::GetID("MyDockspace");
		static std::vector<SharedPtr<EditorPanel>> hiddenPanels;

		//全屏模式下的设置
		if (m_Settings.m_FullScreenSceneView != gameFullScreen)
		{
			m_Settings.m_FullScreenSceneView = gameFullScreen;

			if (m_Settings.m_FullScreenSceneView)
			{
				for (auto panel : m_Panels)
				{
					if (panel->GetSimpleName() != "Game" && panel->Active())
					{
						panel->SetActive(false);
						hiddenPanels.push_back(panel);
					}
				}
			}
			else
			{
				for (auto panel : hiddenPanels)
				{
					panel->SetActive(true);
				}

				hiddenPanels.clear();
			}
		}

		if (!ImGui::DockBuilderGetNode(DockspaceID))
		{
			ImGui::DockBuilderRemoveNode(DockspaceID); // Clear out existing layout
			ImGui::DockBuilderAddNode(DockspaceID);    // Add empty node
			ImGui::DockBuilderSetNodeSize(DockspaceID, ImGui::GetIO().DisplaySize * ImGui::GetIO().DisplayFramebufferScale);

			/*ImGuiID dock_main_id = DockspaceID;
			ImGuiID DockBottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
			ImGuiID DockLeft = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
			ImGuiID DockRight = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);

			ImGuiID DockLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.875f, nullptr, &DockLeft);
			ImGuiID DockRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.875f, nullptr, &DockRight);
			ImGuiID DockingLeftDownChild = ImGui::DockBuilderSplitNode(DockLeftChild, ImGuiDir_Down, 0.06f, nullptr, &DockLeftChild);
			ImGuiID DockingRightDownChild = ImGui::DockBuilderSplitNode(DockRightChild, ImGuiDir_Down, 0.06f, nullptr, &DockRightChild);

			ImGuiID DockBottomChild = ImGui::DockBuilderSplitNode(DockBottom, ImGuiDir_Down, 0.2f, nullptr, &DockBottom);
			ImGuiID DockingBottomLeftChild = ImGui::DockBuilderSplitNode(DockLeft, ImGuiDir_Down, 0.4f, nullptr, &DockLeft);
			ImGuiID DockingBottomRightChild = ImGui::DockBuilderSplitNode(DockRight, ImGuiDir_Down, 0.4f, nullptr, &DockRight);

			ImGuiID DockMiddle = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.8f, nullptr, &dock_main_id);
			ImGuiID DockBottomMiddle = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Down, 0.3f, nullptr, &DockMiddle);
			ImGuiID DockMiddleLeft = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Left, 0.5f, nullptr, &DockMiddle);
			ImGuiID DockMiddleRight = ImGui::DockBuilderSplitNode(DockMiddle, ImGuiDir_Right, 0.5f, nullptr, &DockMiddle);

			ImGui::DockBuilderDockWindow("###game", DockMiddleRight);
			ImGui::DockBuilderDockWindow("###scene", DockMiddleLeft);
			ImGui::DockBuilderDockWindow("###inspector", DockRight);
			ImGui::DockBuilderDockWindow("###console", DockBottomMiddle);
			ImGui::DockBuilderDockWindow("###profiler", DockingBottomLeftChild);
			ImGui::DockBuilderDockWindow("###resources", DockingBottomLeftChild);
			ImGui::DockBuilderDockWindow("Dear ImGui Demo", DockLeft);
			ImGui::DockBuilderDockWindow("GraphicsInfo", DockLeft);
			ImGui::DockBuilderDockWindow("ApplicationInfo", DockLeft);
			ImGui::DockBuilderDockWindow("###hierarchy", DockLeft);
			ImGui::DockBuilderDockWindow("###textEdit", DockMiddleLeft);
			ImGui::DockBuilderDockWindow("###scenesettings", DockLeft);
			ImGui::DockBuilderDockWindow("###editorsettings", DockLeft);
			ImGui::DockBuilderDockWindow("###projectsettings", DockLeft);*/

			ImGui::DockBuilderFinish(DockspaceID);
		}

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGui::DockSpace(DockspaceID, ImVec2(0.0f, 0.0f), opt_flags);
		}
	}

	void Editor::EndDockSpace()
	{
		ImGui::End();
	}

	/// <summary>
	/// 绘制3D 空间中的网格线
	/// </summary>
	void Editor::Draw3DGrid()
	{
		SHAPES_PROFILE_FUNCTION();

		if (!m_GridRenderer || !Application::Get().GetSceneManager()->GetCurrentScene())
		{
			return;
		}

		DebugRenderer::DrawHairLine(glm::vec3(-5000.0f, 0.0f, 0.0f), glm::vec3(5000.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		DebugRenderer::DrawHairLine(glm::vec3(0.0f, -5000.0f, 0.0f), glm::vec3(0.0f, 5000.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		DebugRenderer::DrawHairLine(glm::vec3(0.0f, 0.0f, -5000.0f), glm::vec3(0.0f, 0.0f, 5000.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		m_GridRenderer->OnImGui();

		m_GridRenderer->BeginScene(Application::Get().GetSceneManager()->GetCurrentScene(), m_EditorCamera.get(), &m_EditorCameraTransform);
		m_GridRenderer->RenderScene();
	}

	void Editor::CreateGridRenderer()
	{
		SHAPES_PROFILE_FUNCTION();
		if (!m_GridRenderer)
			m_GridRenderer = CreateSharedPtr<Graphics::GridRenderer>(uint32_t(Application::Get().m_SceneViewWidth), uint32_t(Application::Get().m_SceneViewHeight));
	}

	const SharedPtr<Graphics::GridRenderer>& Editor::GetGridRenderer()
	{
		SHAPES_PROFILE_FUNCTION();
		if (!m_GridRenderer)
			m_GridRenderer = CreateSharedPtr<Graphics::GridRenderer>(uint32_t(Application::Get().m_SceneViewWidth), uint32_t(Application::Get().m_SceneViewHeight));
		return m_GridRenderer;
	}
}