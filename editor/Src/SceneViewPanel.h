#pragma once

#include "EditorPanel.h"

namespace Shapes
{
	// ³¡¾°±à¼­À¸
	class SceneViewPanel :public EditorPanel
	{
	public:
		SceneViewPanel();
		~SceneViewPanel() = default;

		void OnImGui() override;
	};
}