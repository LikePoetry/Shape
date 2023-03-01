// Copyright 
// SJ
// ��Դ����ҳ��,�ڱ༭�� Editor �н��й������

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
