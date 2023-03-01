#pragma once

#include "EditorPanel.h"

#include <Shapes/Core/Application.h>
#include <Shapes/Maths/Transform.h>
#include <Shapes/Utilities/IniFile.h>
#include <Shapes/Graphics/Camera/EditorCamera.h>
#include <Shapes/Graphics/Camera/Camera.h>
#include <Shapes/ImGui/ImGuiUtilities.h>
#include <Shapes/Graphics/Renderers/GridRenderer.h>

#include "FileBrowserPanel.h"

namespace Shapes
{

#define BIND_FILEBROWSER_FN(fn) [this](auto&&... args) -> decltype(auto) { \
    return this->fn(std::forward<decltype(args)>(args)...);                \
}

	class Editor:public Application
	{
		friend class Application;

	public:
		Editor();
		virtual ~Editor();

		void Init() override;
		void OnImGui() override;
		void OnRender() override;
		void OnEvent(Event& e) override;

		void OnUpdate(const TimeStep& ts) override;
		void OnNewScene(Scene* scene) override;

		void DrawMenuBar();
		void BeginDockSpace(bool gameFullScreen);
		void EndDockSpace();

		bool IsTextFile(const std::string& filePath);
		bool IsAudioFile(const std::string& filePath);
		bool IsSceneFile(const std::string& filePath);
		bool IsModelFile(const std::string& filePath);
		bool IsTextureFile(const std::string& filePath);
		bool IsShaderFile(const std::string& filePath);
		bool IsFontFile(const std::string& filePath);

		void Draw3DGrid();
		void CreateGridRenderer();
		const SharedPtr<Graphics::GridRenderer>& GetGridRenderer();

		void OpenFile();
		void EmbedFile();
		const char* GetIconFontIcon(const std::string& fileType);

		void FileOpenCallback(const std::string& filepath);
		void FileEmbedCallback(const std::string& filepath);

		FileBrowserPanel& GetFileBrowserPanel()
		{
			return m_FileBrowserPanel;
		}

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
		void SetSceneViewActive(bool active) { m_SceneViewActive = active; }

	protected:
		NONCOPYABLE(Editor)
			
		uint32_t m_ImGuizmoOperation = 14463;
		entt::entity m_SelectedEntity;

		bool m_SceneViewActive = false;

		std::vector<SharedPtr<EditorPanel>> m_Panels;
		EditorSettings m_Settings;		//编辑器的相关设置操作

		Camera* m_CurrentCamera = nullptr;
		EditorCameraController m_EditorCameraController;

		Maths::Transform m_EditorCameraTransform;
		SharedPtr<Camera> m_EditorCamera = nullptr;

		SharedPtr<Graphics::Texture2D> m_PreviewTexture;
		SharedPtr<Graphics::GridRenderer> m_GridRenderer; //绘制网格线

		SharedPtr<Graphics::Texture2D> m_TestTexture;

		// 文件浏览工具
		FileBrowserPanel m_FileBrowserPanel;
		


		static Editor* s_Editor;

	};
}