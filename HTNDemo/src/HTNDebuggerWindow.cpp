#include "HTNDebuggerWindow.h"

#include "Runtime/HTNPlanningUnit.h"
#include "imgui.h"

#include <filesystem>
#include <vector>

void HTNDebuggerWindow::Render(bool& _IsOpen)
{
    if (!mPlanningUnit)
    {
        return;
    }

    static const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin("HTN Debugger Window", &_IsOpen, WindowFlags))
    {
        static const std::filesystem::path CurrentPath        = std::filesystem::current_path();
        static const std::filesystem::path ParentPath         = CurrentPath.parent_path();
        static const std::string           DomainsSubPathName = "\\Domains";
        static const std::string           DomainsPathName    = ParentPath.string() + DomainsSubPathName;
        static const std::filesystem::path DomainsPath        = std::filesystem::path(DomainsPathName);
        static const std::string           DomainExtension    = ".domain";

        std::vector<std::filesystem::path> DomainPaths;
        for (const std::filesystem::directory_entry& DirectoryEntry : std::filesystem::directory_iterator(DomainsPath))
        {
            const std::filesystem::path& Path = DirectoryEntry.path();
            if (Path.extension() != DomainExtension)
            {
                continue;
            }

            DomainPaths.emplace_back(Path);
        }

        static std::string           SelectedDomainPathName;
        static const ImGuiComboFlags ComboFlags = ImGuiComboFlags_None;
        if (ImGui::BeginCombo("Domains", SelectedDomainPathName.c_str(), ComboFlags))
        {
            for (const std::filesystem::path& DomainPath : DomainPaths)
            {
                static bool                       IsSelected      = (SelectedDomainPathName == DomainPath.string());
                static const ImGuiSelectableFlags SelectableFlags = ImGuiSelectableFlags_None;
                if (ImGui::Selectable(DomainPath.filename().string().c_str(), IsSelected, SelectableFlags))
                {
                    SelectedDomainPathName = DomainPath.string();
                }

                if (IsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        if (ImGui::Button("Parse Domain"))
        {
            mPlanningUnit->ParseDomain(SelectedDomainPathName.c_str());
        }

        ImGui::End();
    }
}
