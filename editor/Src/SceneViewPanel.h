#pragma once

#include "EditorPanel.h"

#include <imgui/imgui.h>
DISABLE_WARNING_PUSH
DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE
#include <entt/entt.hpp>
DISABLE_WARNING_POP

namespace Shapes
{
	// ³¡¾°±à¼­À¸
	class SceneViewPanel :public EditorPanel
	{
	public:
		SceneViewPanel();
		~SceneViewPanel() = default;

		void OnImGui() override;

	private:


		uint32_t m_Width, m_Height;
	};
}