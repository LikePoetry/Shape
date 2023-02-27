#include "SceneViewPanel.h"


#include "Shapes/ImGui/IconsMaterialDesignIcons.h"
#include "Shapes/Core/Application.h"
#include "Shapes/Graphics/Renderers/SceneRenderer.h"

#include <ImGui/imgui.h>

namespace Shapes
{
	SceneViewPanel::SceneViewPanel()
	{
		// 图标和名称串联在一起
		m_Name = ICON_MDI_GAMEPAD_VARIANT " Scene###scene";
		m_SimpleName = "Scene";


		m_Width = 1280;
		m_Height = 800;

		Application::Get().GetSceneRenderer()->SetDisablePostProcess(false);
	}

	void SceneViewPanel::OnImGui()
	{
		SHAPES_PROFILE_FUNCTION();

		Application& app = Application::Get();


		ImGui::Begin(m_Name.c_str());
		//ImGui::Image()
		ImGui::End();
	}
}