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

	protected:
		NONCOPYABLE(Editor)

		std::vector<SharedPtr<EditorPanel>> m_Panels;
		
		static Editor* s_Editor;

	};
}