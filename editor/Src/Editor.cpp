#include "Editor.h"

#include "Shapes/Core/OS/FileSystem.h"
#include "Shapes/Core/StringUtilities.h"

#include "SceneViewPanel.h"


#include <imgui/imgui.h>

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

		// ÃÌº” ”Õº“≥√Ê
		m_Panels.emplace_back(CreateSharedPtr<SceneViewPanel>());

		for (auto& panel : m_Panels)
			panel->SetEditor(this);
	}

	void Editor::OnImGui()
	{
		ImGui::Begin("Metrics");
		ImGui::Text("FPS : %.2f", (float)Shapes::Engine::Get().Statistics().FramesPerSecond);
		ImGui::End();

		for (auto& panel : m_Panels)
			panel->OnImGui();

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
}