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

		void  OnImGui() override;
		bool RenderFile(int dirIndex, bool folder, int showIndex, bool gridView);
		void DrawFolder(const SharedPtr<DirectoryInformation>& dirInfo, bool defaultOpen = false);
		void RenderBreadCrumbs();
		void RenderBottom();
		void GetDirectories(const std::string& path);

		int GetParsedAssetID(const std::string& extension)
		{
			for (int i = 0; i < assetTypes.size(); i++)
			{
				if (extension == assetTypes[i])
				{
					return i;
				}
			}

			return -1;
		}

		static std::string GetParentPath(const std::string& path);

		static std::vector<std::string> SearchFiles(const std::string& query);
		static bool MoveFile(const std::string& filePath, const std::string& movePath);

		std::string StripExtras(const std::string& filename);
		std::string ProcessDirectory(const std::filesystem::path& directoryPath, const SharedPtr<DirectoryInformation>& parent);

		void ChangeDirectory(SharedPtr<DirectoryInformation>& directory);
		void RemoveDirectory(SharedPtr<DirectoryInformation>& directory, bool removeFromParent = true);
		void OnNewProject() override;
		void Refresh();

	private:
		static inline std::vector<std::string> assetTypes = {
			"fbx", "obj", "wav", "cs", "png", "blend", "lsc", "ogg", "lua"
		};

		std::string m_MovePath;
		std::string m_LastNavPath;
		static std::string m_Delimiter;

		size_t m_BasePathLen;
		bool m_IsDragging;
		bool m_IsInListView;
		bool m_UpdateBreadCrumbs;
		bool m_ShowHiddenFiles;
		int m_GridItemsPerRow;
		float m_GridSize = 50.0f;

		ImGuiTextFilter m_Filter;

		char* inputText;
		char* inputHint;
		char inputBuffer[1024];

		std::string m_BasePath;

		bool m_UpdateNavigationPath = true;

		SharedPtr<DirectoryInformation> m_CurrentDir;
		SharedPtr<DirectoryInformation> m_BaseProjectDir;
		SharedPtr<DirectoryInformation> m_NextDirectory;
		SharedPtr<DirectoryInformation> m_PreviousDirectory;
		std::unordered_map<std::string, SharedPtr<DirectoryInformation>> m_Directories;
		std::vector<SharedPtr<DirectoryInformation>> m_BreadCrumbData;
	};
}
