#include "Editor.h"

#include "Shapes/Core/OS/FileSystem.h"
#include "Shapes/Core/StringUtilities.h"
#include "Shapes/ImGui/ImGuiUtilities.h"

#include "SceneViewPanel.h"


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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

		// 添加视图页面
		m_Panels.emplace_back(CreateSharedPtr<SceneViewPanel>());

		for (auto& panel : m_Panels)
			panel->SetEditor(this);
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

		EndDockSpace();

		Application::OnImGui();
	}

	void Editor::OnRender()
	{
		Application::OnRender();
	}

	void Editor::OnUpdate(const TimeStep& ts)
	{
		Application::OnUpdate(ts);
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

}