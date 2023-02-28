#pragma once

#include "EditorPanel.h"
#include <Shapes/Core/Application.h>
#include <Shapes/Maths/Transform.h>
#include <Shapes/Utilities/IniFile.h>
#include <Shapes/Graphics/Camera/EditorCamera.h>
#include <Shapes/Graphics/Camera/Camera.h>
#include <Shapes/ImGui/ImGuiUtilities.h>
#include <Shapes/Graphics/Renderers/GridRenderer.h>

namespace Shapes
{
	class Editor:public Application
	{
		friend class Application;

	public:
		Editor();
		virtual ~Editor();

		void Init() override;
		void OnImGui() override;
		void OnRender() override;

		void OnUpdate(const TimeStep& ts) override;
		void OnNewScene(Scene* scene) override;

		void DrawMenuBar();
		void BeginDockSpace(bool gameFullScreen);
		void EndDockSpace();

		void Draw3DGrid();
		void CreateGridRenderer();
		const SharedPtr<Graphics::GridRenderer>& GetGridRenderer();

		Maths::Transform& GetEditorCameraTransform()
		{
			return m_EditorCameraTransform;
		}

		Camera* GetCamera() const
		{
			return m_EditorCamera.get();
		}

		glm::vec2 m_SceneViewPanelPosition;

		struct EditorSettings
		{
			float m_GridSize = 10.0f;
			uint32_t m_DebugDrawFlags = 0;
			uint32_t m_Physics2DDebugFlags = 0;
			uint32_t m_Physics3DDebugFlags = 0;

			bool m_ShowGrid = true;
			bool m_ShowGizmos = true;
			bool m_ShowViewSelected = false;
			bool m_SnapQuizmo = false;
			bool m_ShowImGuiDemo = true;
			bool m_View2D = false;
			bool m_FullScreenOnPlay = false;
			float m_SnapAmount = 1.0f;
			bool m_SleepOutofFocus = true;
			float m_ImGuizmoScale = 0.25f;

			bool m_FullScreenSceneView = false;
			ImGuiUtilities::Theme m_Theme = ImGuiUtilities::Theme::Black;
			bool m_FreeAspect = true;
			float m_FixedAspect = 1.0f;
			bool m_HalfRes = false;
			float m_AspectRatio = 1.0f;

			// Camera Settings
			float m_CameraSpeed = 1000.0f;
			float m_CameraNear = 0.01f;
			float m_CameraFar = 1000.0f;

			std::vector<std::string> m_RecentProjects;
		};

		EditorSettings& GetSettings() { return m_Settings; }
	protected:
		NONCOPYABLE(Editor)

		std::vector<SharedPtr<EditorPanel>> m_Panels;
		EditorSettings m_Settings;		//编辑器的相关设置操作


		Camera* m_CurrentCamera = nullptr;
		Maths::Transform m_EditorCameraTransform;
		SharedPtr<Camera> m_EditorCamera = nullptr;

		SharedPtr<Graphics::GridRenderer> m_GridRenderer; //绘制网格线
		
		static Editor* s_Editor;

	};
}