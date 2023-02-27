#pragma once

#include "EditorPanel.h"
#include <Shapes/Core/Application.h>

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

		void DrawMenuBar();
		void BeginDockSpace(bool gameFullScreen);
		void EndDockSpace();

	protected:
		NONCOPYABLE(Editor)

		std::vector<SharedPtr<EditorPanel>> m_Panels;
		
		static Editor* s_Editor;

	};
}