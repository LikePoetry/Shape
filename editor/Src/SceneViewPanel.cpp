#include "SceneViewPanel.h"


#include "Shapes/ImGui/IconsMaterialDesignIcons.h"
#include "Shapes/ImGui/ImGuiUtilities.h"
#include "Shapes/Core/Application.h"
#include "Shapes/Graphics/Renderers/SceneRenderer.h"
#include "Shapes/Events/ApplicationEvent.h"


#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/Plugins/ImGuizmo.h>

namespace Shapes
{
	SceneViewPanel::SceneViewPanel()
	{
		// 图标和名称串联在一起
		m_Name = ICON_MDI_GAMEPAD_VARIANT " Scene###scene";
		m_SimpleName = "Scene";
		m_CurrentScene = nullptr;

		m_Width = 1280;
		m_Height = 800;

		Application::Get().GetSceneRenderer()->SetDisablePostProcess(false);
	}

	void SceneViewPanel::OnImGui()
	{
		SHAPES_PROFILE_FUNCTION();

		Application& app = Application::Get();

		ImGuiUtilities::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

		if (!ImGui::Begin(m_Name.c_str(), &m_Active, flags) || !m_CurrentScene)
		{
			app.SetDisableMainSceneRenderer(true);
			ImGui::End();
			return;
		}

		Camera* camera = nullptr;
		Maths::Transform* transform = nullptr;

		app.SetDisableMainSceneRenderer(false);

		// if(app.GetEditorState() == EditorState::Preview)
		{
			//**************************
			// 重要的几行代码，一定要将原场景中的相机设置好！关联到Shader
			//**************************
			SHAPES_PROFILE_SCOPE("Set Override Camera");
			camera = m_Editor->GetCamera();
			transform = &m_Editor->GetEditorCameraTransform();
			app.GetSceneRenderer()->SetOverrideCamera(camera, transform);
		}

		//TODO 
		ImVec2 offset = { 0.0f, 0.0f };

		{
			// 绘制编辑窗口工具栏
			ToolBar();
			offset = ImGui::GetCursorPos(); // Usually ImVec2(0.0f,50.0f);
		}

		if (!camera)
		{
			ImGui::End();
			return;
		}

		ImGuizmo::SetDrawlist();

		auto sceneViewSize = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() - offset * 0.5f; // - offset * 0.5f;
		auto sceneViewPosition = ImGui::GetWindowPos() + offset;

		sceneViewSize.x -= static_cast<int>(sceneViewSize.x) % 2 != 0 ? 1.0f : 0.0f;
		sceneViewSize.y -= static_cast<int>(sceneViewSize.y) % 2 != 0 ? 1.0f : 0.0f;

		// 纵横比
		float aspect = static_cast<float>(sceneViewSize.x) / static_cast<float>(sceneViewSize.y);

		// 设置相机的纵横比
		if (!Maths::Equals(aspect, camera->GetAspectRatio()))
		{
			camera->SetAspectRatio(aspect);
		}

		m_Editor->m_SceneViewPanelPosition = glm::vec2(sceneViewPosition.x, sceneViewPosition.y);

		if (m_Editor->GetSettings().m_HalfRes)
			sceneViewSize *= 0.5f;

		sceneViewSize.x -= static_cast<int>(sceneViewSize.x) % 2 != 0 ? 1.0f : 0.0f;
		sceneViewSize.y -= static_cast<int>(sceneViewSize.y) % 2 != 0 ? 1.0f : 0.0f;

		Resize(static_cast<uint32_t>(sceneViewSize.x), static_cast<uint32_t>(sceneViewSize.y));

		if (m_Editor->GetSettings().m_HalfRes)
			sceneViewSize *= 2.0f;

		// 将纹理数据取出绘制
		ImGui::Image(m_GameViewTexture.get(), glm::vec2(sceneViewSize.x, sceneViewSize.y));

		// 确定鼠标是否悬浮在Sceneview上操作 sceneview
		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = sceneViewPosition;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		bool updateCamera = ImGui::IsMouseHoveringRect(minBound, maxBound); // || Input::Get().GetMouseMode() == MouseMode::Captured;

		app.SetSceneActive(ImGui::IsWindowFocused() && !ImGuizmo::IsUsing() && updateCamera);

		ImGuizmo::SetRect(sceneViewPosition.x, sceneViewPosition.y, sceneViewSize.x, sceneViewSize.y);

		m_Editor->SetSceneViewActive(updateCamera);


		ImGui::End();
	}

	void SceneViewPanel::OnNewScene(Scene* scene)
	{
		SHAPES_PROFILE_FUNCTION();
		m_Editor->GetSettings().m_AspectRatio = 1.0f;
		m_CurrentScene = scene;

		auto SceneRenderer = Application::Get().GetSceneRenderer();
		SceneRenderer->SetRenderTarget(m_GameViewTexture.get(), true);
		SceneRenderer->SetOverrideCamera(m_Editor->GetCamera(), &m_Editor->GetEditorCameraTransform());
		m_Editor->GetGridRenderer()->SetRenderTarget(m_GameViewTexture.get(), true);
		m_Editor->GetGridRenderer()->SetDepthTarget(SceneRenderer->GetForwardData().m_DepthTexture);
	}

	/// <summary>
	/// 绘制窗口工具栏
	/// </summary>
	void SceneViewPanel::ToolBar()
	{

	}

	void SceneViewPanel::Resize(uint32_t width, uint32_t height)
	{
		SHAPES_PROFILE_FUNCTION();

		bool resize = false;

		SHAPES_ASSERT(width > 0 && height > 0, "Scene View Dimensions 0");

		Application::Get().SetSceneViewDimensions(width, height);

		if (m_Width != width || m_Height != height)
		{
			resize = true;
			m_Width = width;
			m_Height = height;
		}

		if (!m_GameViewTexture)
		{
			Graphics::TextureDesc mainRenderTargetDesc;
			mainRenderTargetDesc.format = Graphics::RHIFormat::R8G8B8A8_Unorm;
			mainRenderTargetDesc.flags = Graphics::TextureFlags::Texture_RenderTarget;

			m_GameViewTexture = SharedPtr<Graphics::Texture2D>(Graphics::Texture2D::Create(mainRenderTargetDesc, m_Width, m_Height));
		}

		if (resize)
		{
			m_GameViewTexture->Resize(m_Width, m_Height);

			auto SceneRenderer = Application::Get().GetSceneRenderer();
			SceneRenderer->SetRenderTarget(m_GameViewTexture.get(), true, false);

			if (!m_Editor->GetGridRenderer())
				m_Editor->CreateGridRenderer();
			m_Editor->GetGridRenderer()->SetRenderTarget(m_GameViewTexture.get(), false);
			m_Editor->GetGridRenderer()->SetDepthTarget(SceneRenderer->GetForwardData().m_DepthTexture);

			WindowResizeEvent e(width, height);
			auto& app = Application::Get();
			app.GetSceneRenderer()->OnResize(width, height);

			SceneRenderer->OnEvent(e);

			m_Editor->GetGridRenderer()->OnResize(m_Width, m_Height);

		}
	}
}