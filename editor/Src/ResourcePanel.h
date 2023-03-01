// Copyright 
// SJ
// 资源管理页面,在编辑器 Editor 中进行管理调用

#pragma once

#include "EditorPanel.h"

#include <Shapes/Core/Reference.h>

#if __has_include (<filesystem>)
#include <filesystem>
#endif // __has_include (<filesystem>)

namespace Shapes
{
	struct DirectoryInformation
	{
		SharedPtr<DirectoryInformation> Parent;
		std::vector<SharedPtr<DirectoryInformation>> Children;

		std::filesystem::path FilePath;
		bool IsFile;

	public:
		DirectoryInformation(const std::filesystem::path& fname, bool isF)
		{
			FilePath = fname;
			IsFile = isF;
		}
	};

	class ResourcePanel :public EditorPanel
	{
	public:
		ResourcePanel();
		~ResourcePanel() = default;


	};
}
