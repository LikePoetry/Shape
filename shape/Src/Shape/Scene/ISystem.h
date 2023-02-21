#pragma once
#include "Shape/Scene/Scene.h"

namespace Shape
{
	class TimeStep;

	class SHAPE_EXPORT ISystem
	{
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual void OnInit() = 0;
		virtual void OnUpdate(const TimeStep& dt, Scene* scene) = 0;
		virtual void OnImGui() = 0;
		virtual void OnDebugDraw() = 0;

		inline const std::string& GetName() const
		{
			return m_DebugName;
		}

	protected:
		std::string m_DebugName;
	};
}