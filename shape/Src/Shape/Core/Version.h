#pragma once
namespace Shape
{
	struct Version
	{
		int major = 0;
		int minor = 3;
		int patch = 7;
	};

	constexpr Version const ShapeVersion = Version();
}