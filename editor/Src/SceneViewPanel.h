#pragma once

#include "EditorPanel.h"

namespace Shapes
{
	// �����༭��
	class SceneViewPanel :public EditorPanel
	{
	public:
		SceneViewPanel();
		~SceneViewPanel() = default;

		void OnImGui() override;
	};
}