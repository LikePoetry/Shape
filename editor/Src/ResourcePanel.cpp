// Copyright 
// SJ
// ��Դ����ҳ��

#include "Editor.h"
#include "ResourcePanel.h"

#include <Shapes/Core/OS/FileSystem.h>
#include <Shapes/Core/Profiler.h>
#include <Shapes/Core/StringUtilities.h>
#include <Shapes/Core/VFS.h>


#include <Shapes/ImGui/IconsMaterialDesignIcons.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Shapes
{
	std::string ResourcePanel::m_Delimiter = "\\";

	ResourcePanel::ResourcePanel()
	{
		SHAPES_PROFILE_FUNCTION();

		m_Name = ICON_MDI_FOLDER_STAR " Resources###resources";
		m_SimpleName = "Resources";

		m_BasePath = Application::Get().GetProjectSettings().m_ProjectRoot + "Assets";
		std::string baseDirectoryHandle = ProcessDirectory(std::filesystem::path(m_BasePath), nullptr);
		m_BaseProjectDir = m_Directories[baseDirectoryHandle];
		ChangeDirectory(m_BaseProjectDir);

		m_CurrentDir = m_BaseProjectDir;

		m_UpdateNavigationPath = true;
		m_IsDragging = false;
		m_IsInListView = true;
		m_UpdateBreadCrumbs = true;
		m_ShowHiddenFiles = false;
	}

	/// <summary>
	/// �л��ļ���
	/// </summary>
	/// <param name="directory"></param>
	void ResourcePanel::ChangeDirectory(SharedPtr<DirectoryInformation>& directory)
	{
		if (!directory)
			return;

		m_PreviousDirectory = m_CurrentDir;
		m_CurrentDir = directory;
		m_UpdateNavigationPath = true;
	}

	/// <summary>
	/// �Ƴ�ĳ���ļ���
	/// </summary>
	/// <param name="directory"></param>
	/// <param name="removeFromParent"></param>
	void ResourcePanel::RemoveDirectory(SharedPtr<DirectoryInformation>& directory, bool removeFromParent)
	{
		if (directory->Parent && removeFromParent)
		{
			directory->Parent->Children.clear();
		}

		for (auto& subdir : directory->Children)
			RemoveDirectory(subdir, false);

		m_Directories.erase(m_Directories.find(directory->FilePath.string()));
	}

	std::string ResourcePanel::ProcessDirectory(const std::filesystem::path& directoryPath, const SharedPtr<DirectoryInformation>& parent)
	{
		const auto& directory = m_Directories[directoryPath.string()];
		if (directory)
			return directory->FilePath.string();

		SharedPtr<DirectoryInformation> directoryInfo = CreateSharedPtr<DirectoryInformation>(directoryPath, !std::filesystem::is_directory(directoryPath));
		directoryInfo->Parent = parent;

		if (directoryPath == m_BasePath)
			directoryInfo->FilePath = m_BasePath;
		else
			directoryInfo->FilePath = std::filesystem::relative(directoryPath, m_BasePath);

		if (std::filesystem::is_directory(directoryPath))
		{
			for (auto entry : std::filesystem::directory_iterator(directoryPath))
			{
				if (!m_ShowHiddenFiles && Shapes::StringUtilities::IsHiddenFile(entry.path().string()))
				{
					continue;
				}
				{
					std::string subdirHandle = ProcessDirectory(entry.path(), directoryInfo);
					directoryInfo->Children.push_back(m_Directories[subdirHandle]);
				}
			}
		}

		m_Directories[directoryInfo->FilePath.string()] = directoryInfo;
		return directoryInfo->FilePath.string();
	}

	/// <summary>
	/// �ļ��л�����ʽ
	/// </summary>
	/// <param name="dirInfo"></param>
	/// <param name="defaultOpen"></param>
	void ResourcePanel::DrawFolder(const SharedPtr<DirectoryInformation>& dirInfo, bool defaultOpen)
	{
		SHAPES_PROFILE_FUNCTION();
		ImGuiTreeNodeFlags nodeFlags = ((dirInfo == m_CurrentDir) ? ImGuiTreeNodeFlags_Selected : 0);
		nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		if (dirInfo->Parent == nullptr)
			nodeFlags |= ImGuiTreeNodeFlags_Framed;

		const ImColor TreeLineColor = ImColor(128, 128, 128, 128);
		const float SmallOffsetX = 6.0f * Application::Get().GetWindowDPI();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		if (!dirInfo->IsFile)
		{
			bool containsFolder = false;

			for (auto& file : dirInfo->Children)
			{
				if (!file->IsFile)
				{
					containsFolder = true;
					break;
				}
			}
			if (!containsFolder)
				nodeFlags |= ImGuiTreeNodeFlags_Leaf;

			if (defaultOpen)
				nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf;

			nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth;

			bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)(dirInfo.get()), nodeFlags, "");

			const char* folderIcon = ((isOpen && containsFolder) || m_CurrentDir == dirInfo) ? ICON_MDI_FOLDER_OPEN : ICON_MDI_FOLDER;
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImGuiUtilities::GetIconColour());
			ImGui::Text("%s ", folderIcon);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::TextUnformatted((dirInfo->FilePath.filename().string()).c_str());

			ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

			if (ImGui::IsItemClicked())
			{
				m_PreviousDirectory = m_CurrentDir;
				m_CurrentDir = dirInfo;
				m_UpdateNavigationPath = true;
			}

			if (isOpen && containsFolder)
			{
				verticalLineStart.x += SmallOffsetX; // to nicely line up with the arrow symbol
				ImVec2 verticalLineEnd = verticalLineStart;

				for (int i = 0; i < dirInfo->Children.size(); i++)
				{
					if (!dirInfo->Children[i]->IsFile)
					{
						auto currentPos = ImGui::GetCursorScreenPos();

						ImGui::Indent(10.0f);

						bool containsFolderTemp = false;
						for (auto& file : dirInfo->Children[i]->Children)
						{
							if (!file->IsFile)
							{
								containsFolderTemp = true;
								break;
							}
						}
						float HorizontalTreeLineSize = 16.0f * Application::Get().GetWindowDPI(); // chosen arbitrarily

						if (containsFolderTemp)
							HorizontalTreeLineSize *= 0.5f;
						DrawFolder(dirInfo->Children[i]);

						const ImRect childRect = ImRect(currentPos, currentPos + ImVec2(0.0f, ImGui::GetFontSize()));

						const float midpoint = (childRect.Min.y + childRect.Max.y) * 0.5f;
						drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), TreeLineColor);
						verticalLineEnd.y = midpoint;

						ImGui::Unindent(10.0f);
					}
				}

				drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);

				ImGui::TreePop();
			}

			if (isOpen && !containsFolder)
				ImGui::TreePop();
		}

		if (m_IsDragging && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			m_MovePath = dirInfo->FilePath.string().c_str();
		}

	}

	/// <summary>
	/// ҳ����Ʋ��ִ���
	/// </summary>
	void ResourcePanel::OnImGui()
	{
		SHAPES_PROFILE_FUNCTION();

		ImGui::Begin(m_Name.c_str(), &m_Active);
		{
			auto windowSize = ImGui::GetWindowSize();
			bool vertical = windowSize.y > windowSize.x;

			if (!vertical)
			{
				ImGui::BeginColumns("ResourcePanelColumns", 2, ImGuiOldColumnFlags_NoResize);
				ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionMax().x / 3.0f);
				ImGui::BeginChild("##folders_common");
			}
			else
			{
				ImGui::BeginChild("##folders_common", ImVec2(0, ImGui::GetWindowHeight() / 3.0f));
			}

            {
                RenderBreadCrumbs();

                {
                    ImGui::BeginChild("##folders");
                    {
                        DrawFolder(m_BaseProjectDir, true);
                    }
                    ImGui::EndChild();
                }
            }

            ImGui::EndChild();

            if (ImGui::BeginDragDropTarget())
            {
                auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
                if (data)
                {
                    std::string file = (char*)data->Data;
                    if (MoveFile(file, m_MovePath))
                    {
                        SHAPES_LOG_INFO("Moved File: " + file + " to " + m_MovePath);
                    }
                    m_IsDragging = false;
                }
                ImGui::EndDragDropTarget();
            }
            float offset = 0.0f;
            if (!vertical)
            {
                ImGui::NextColumn();
            }
            else
            {
                offset = ImGui::GetWindowHeight() / 3.0f + 6.0f;
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
            }

            ImGui::BeginChild("##directory_structure", ImVec2(0, ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() * 2.6f - offset));
            {
                {
                    ImGui::BeginChild("##directory_breadcrumbs", ImVec2(ImGui::GetColumnWidth(), ImGui::GetFrameHeightWithSpacing()));

                    if (ImGui::Button(ICON_MDI_ARROW_LEFT))
                    {
                        if (m_CurrentDir != m_BaseProjectDir)
                        {
                            m_PreviousDirectory = m_CurrentDir;
                            m_CurrentDir = m_CurrentDir->Parent;
                            m_UpdateNavigationPath = true;
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(ICON_MDI_ARROW_RIGHT))
                    {
                        m_PreviousDirectory = m_CurrentDir;
                        // m_CurrentDir = m_LastNavPath;
                        m_UpdateNavigationPath = true;
                    }
                    ImGui::SameLine();

                    if (m_UpdateNavigationPath)
                    {
                        m_BreadCrumbData.clear();
                        auto current = m_CurrentDir;
                        while (current)
                        {
                            if (current->Parent != nullptr)
                            {
                                m_BreadCrumbData.push_back(current);
                                current = current->Parent;
                            }
                            else
                            {
                                m_BreadCrumbData.push_back(m_BaseProjectDir);
                                current = nullptr;
                            }
                        }

                        std::reverse(m_BreadCrumbData.begin(), m_BreadCrumbData.end());
                        m_UpdateNavigationPath = false;
                    }

                    if (m_IsInListView)
                    {
                        if (ImGui::Button(ICON_MDI_VIEW_GRID))
                        {
                            m_IsInListView = !m_IsInListView;
                        }
                        ImGui::SameLine();
                    }
                    else
                    {
                        if (ImGui::Button(ICON_MDI_VIEW_LIST))
                        {
                            m_IsInListView = !m_IsInListView;
                        }
                        ImGui::SameLine();
                    }

                    ImGui::TextUnformatted(ICON_MDI_MAGNIFY);
                    ImGui::SameLine();

                    m_Filter.Draw("##Filter", ImGui::GetContentRegionAvail().x - ImGui::GetStyle().IndentSpacing);

                    ImGui::EndChild();
                }

                {
                    ImGui::BeginChild("##Scrolling");

                    int shownIndex = 0;

                    float xAvail = ImGui::GetContentRegionAvail().x;

                    m_GridItemsPerRow = (int)floor(xAvail / (m_GridSize + ImGui::GetStyle().ItemSpacing.x));
                    m_GridItemsPerRow = Maths::Max(1, m_GridItemsPerRow);

                    if (m_IsInListView)
                    {
                        for (int i = 0; i < m_CurrentDir->Children.size(); i++)
                        {
                            if (m_CurrentDir->Children.size() > 0)
                            {
                                if (!m_ShowHiddenFiles && Shapes::StringUtilities::IsHiddenFile(m_CurrentDir->Children[i]->FilePath.filename().string()))
                                {
                                    continue;
                                }

                                if (m_Filter.IsActive())
                                {
                                    if (!m_Filter.PassFilter(m_CurrentDir->Children[i]->FilePath.filename().string().c_str()))
                                    {
                                        continue;
                                    }
                                }

                                bool doubleClicked = RenderFile(i, !m_CurrentDir->Children[i]->IsFile, shownIndex, !m_IsInListView);

                                if (doubleClicked)
                                    break;
                                shownIndex++;
                            }
                        }
                    }
                    else
                    {
                        for (int i = 0; i < m_CurrentDir->Children.size(); i++)
                        {
                            if (!m_ShowHiddenFiles && Shapes::StringUtilities::IsHiddenFile(m_CurrentDir->Children[i]->FilePath.filename().string()))
                            {
                                continue;
                            }

                            if (m_Filter.IsActive())
                            {
                                if (!m_Filter.PassFilter(m_CurrentDir->Children[i]->FilePath.filename().string().c_str()))
                                {
                                    continue;
                                }
                            }

                            bool doubleClicked = RenderFile(i, !m_CurrentDir->Children[i]->IsFile, shownIndex, !m_IsInListView);

                            if (doubleClicked)
                                break;
                            shownIndex++;
                        }
                    }

                    if (ImGui::BeginPopupContextWindow())
                    {
                        {
                            if (ImGui::Selectable("Import New Asset"))
                            {
                                m_Editor->OpenFile();
                            }

                            if (ImGui::Selectable("Refresh"))
                            {
                                Refresh();
                            }

                            if (ImGui::Selectable("New folder"))
                            {
                                std::filesystem::create_directory(std::filesystem::path(m_BasePath + "/" + m_CurrentDir->FilePath.string() + "/NewFolder"));
                                Refresh();
                            }
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::EndChild();
                }

                ImGui::EndChild();

                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
                RenderBottom();
            }

            if (ImGui::BeginDragDropTarget())
            {
                auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
                if (data)
                {
                    std::string a = (char*)data->Data;
                    if (MoveFile(a, m_MovePath))
                    {
                        SHAPES_LOG_INFO("Moved File: " + a + " to " + m_MovePath);
                    }
                    m_IsDragging = false;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::End();
        }

	}

    /// <summary>
    /// �������м������
    /// </summary>
    void ResourcePanel::RenderBreadCrumbs()
    {

    }

    bool ResourcePanel::RenderFile(int dirIndex, bool folder, int shownIndex, bool gridView)
    {
        SHAPES_PROFILE_FUNCTION();
        auto fileID = GetParsedAssetID(StringUtilities::GetFilePathExtension(m_CurrentDir->Children[dirIndex]->FilePath.string()));

        bool doubleClicked = false;

        if (gridView)
        {
            ImGui::BeginGroup();

            if (ImGui::Button(folder ? ICON_MDI_FOLDER : m_Editor->GetIconFontIcon(m_CurrentDir->Children[dirIndex]->FilePath.string()), ImVec2(m_GridSize, m_GridSize)))
            {
            }

            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                doubleClicked = true;
            }

            auto newFname = StripExtras(m_CurrentDir->Children[dirIndex]->FilePath.filename().string());

            ImGui::TextUnformatted(newFname.c_str());
            ImGui::EndGroup();

            if ((shownIndex + 1) % m_GridItemsPerRow != 0)
                ImGui::SameLine();
        }
        else
        {
            ImGui::TextUnformatted(folder ? ICON_MDI_FOLDER : m_Editor->GetIconFontIcon(m_CurrentDir->Children[dirIndex]->FilePath.string()));
            ImGui::SameLine();
            if (ImGui::Selectable(m_CurrentDir->Children[dirIndex]->FilePath.filename().string().c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    doubleClicked = true;
                }
            }
        }

        ImGuiUtilities::Tooltip(m_CurrentDir->Children[dirIndex]->FilePath.filename().string());

        if (doubleClicked)
        {
            if (folder)
            {
                m_PreviousDirectory = m_CurrentDir;
                m_CurrentDir = m_CurrentDir->Children[dirIndex];
                m_UpdateNavigationPath = true;
            }
            else
            {
                m_Editor->FileOpenCallback(m_BasePath + "/" + m_CurrentDir->Children[dirIndex]->FilePath.string());
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::TextUnformatted(m_Editor->GetIconFontIcon(m_CurrentDir->Children[dirIndex]->FilePath.string()));

            ImGui::SameLine();
            m_MovePath = m_BasePath + "/" + m_CurrentDir->Children[dirIndex]->FilePath.string();
            ImGui::TextUnformatted(m_MovePath.c_str());
            size_t size = sizeof(const char*) + strlen(m_MovePath.c_str());
            ImGui::SetDragDropPayload("AssetFile", m_MovePath.c_str(), size);
            m_IsDragging = true;
            ImGui::EndDragDropSource();
        }

        return doubleClicked;
    }

    void ResourcePanel::RenderBottom()
    {
        SHAPES_PROFILE_FUNCTION();
        ImGui::BeginChild("##nav", ImVec2(ImGui::GetColumnWidth(), ImGui::GetFontSize() * 1.8f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        {
            int secIdx = 0, newPwdLastSecIdx = -1;

            std::string assetsBasePath;
            VFS::Get().ResolvePhysicalPath("//Assets", assetsBasePath);
            auto dir = std::filesystem::path(assetsBasePath);
            auto& AssetsDir = m_CurrentDir->FilePath;

            size_t PhysicalPathCount = 0;

            for (auto& sec : dir)
            {
                PhysicalPathCount++;
            }
            int dirIndex = 0;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.7f, 0.0f));

            for (auto& directory : m_BreadCrumbData)
            {
                const std::string& directoryName = directory->FilePath.filename().string();
                if (ImGui::SmallButton(directoryName.c_str()))
                    ChangeDirectory(directory);

                ImGui::SameLine();
            }

            ImGui::PopStyleColor();

            if (newPwdLastSecIdx >= 0)
            {
                int i = 0;
                std::filesystem::path newPwd;
                for (auto& sec : AssetsDir)
                {
                    if (i++ > newPwdLastSecIdx)
                        break;
                    newPwd /= sec;
                }
#ifdef _WIN32
                if (newPwdLastSecIdx == 0)
                    newPwd /= "\\";
#endif

                m_PreviousDirectory = m_CurrentDir;
                m_CurrentDir = m_Directories[newPwd.string()];
                m_UpdateNavigationPath = true;
            }

            ImGui::SameLine();
        }

        if (!m_IsInListView)
        {
            ImGui::SliderFloat("##GridSize", &m_GridSize, 40.0f, 400.0f);
        }
        ImGui::EndChild();
    }

    std::vector<std::string> ResourcePanel::SearchFiles(const std::string& query)
    {
        return std::vector<std::string>();
    }

    bool ResourcePanel::MoveFile(const std::string& filePath, const std::string& movePath)
    {
        SHAPES_PROFILE_FUNCTION();
        std::string s = "move " + filePath + " " + movePath.c_str();
#ifndef SHAPES_PLATFORM_IOS
        system(s.c_str());
#endif

        return std::filesystem::exists(std::filesystem::path(movePath + m_Delimiter + StringUtilities::GetFileName(filePath)));
    }

    std::string ResourcePanel::StripExtras(const std::string& filename)
    {
        SHAPES_PROFILE_FUNCTION();
        std::vector<std::string> out;
        size_t start;
        size_t end = 0;

        while ((start = filename.find_first_not_of(".", end)) != std::string::npos)
        {
            end = filename.find(".", start);
            out.push_back(filename.substr(start, end - start));
        }

        int maxChars = int(m_GridSize / (ImGui::GetFontSize() * 0.5f));

        if (out[0].length() >= maxChars)
        {
            auto cutFilename = "     " + out[0].substr(0, maxChars - 3) + "...";
            return cutFilename;
        }

        auto filenameLength = out[0].length();
        //        auto paddingToAdd = maxChars - 1 - filenameLength;
        //
        std::string newFileName;
        //
        //        for(int i = 0; i <= paddingToAdd; i++)
        //        {
        //            newFileName += " ";
        //        }

        newFileName += out[0];

        return newFileName;
    }

    void ResourcePanel::OnNewProject()
    {
        Refresh();
    }

    void ResourcePanel::Refresh()
    {
        m_BasePath = Application::Get().GetProjectSettings().m_ProjectRoot + "Assets";

        auto currentPath = m_CurrentDir->FilePath;

        m_UpdateNavigationPath = true;

        m_Directories.clear();
        std::string baseDirectoryHandle = ProcessDirectory(std::filesystem::path(m_BasePath), nullptr);
        m_BaseProjectDir = m_Directories[baseDirectoryHandle];
        m_PreviousDirectory = nullptr;
        m_CurrentDir = nullptr;

        if (m_Directories.find(currentPath.string()) != m_Directories.end())
            m_CurrentDir = m_Directories[currentPath.string()];
        else
            ChangeDirectory(m_BaseProjectDir);
    }
}