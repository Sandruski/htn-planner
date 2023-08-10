#include "HTNDebuggerWindow.h"

#include "Runtime/HTNAtom.h"
#include "Runtime/HTNPlanningUnit.h"
#include "Runtime/HTNWorldState.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <filesystem>
#include <string>
#include <vector>

namespace
{
    const ImGuiWindowFlags     WindowFlags     = ImGuiWindowFlags_NoCollapse;
    const ImGuiTabBarFlags     TabBarFlags     = ImGuiTabBarFlags_None;
    const ImGuiComboFlags      ComboFlags      = ImGuiComboFlags_None;
    const ImGuiSelectableFlags SelectableFlags = ImGuiSelectableFlags_None;
    const ImGuiTableFlags      TableFlags      = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders;
    const ImGuiInputTextFlags  InputTextFlags  = ImGuiInputTextFlags_CharsNoBlank;

    const ImVec4 FailColor    = ImVec4(1.f, 0.f, 0.f, 1.f);
    const ImVec4 SuccessColor = ImVec4(0.f, 1.f, 0.f, 1.f);
}  // namespace

void HTNDebuggerWindow::Render(bool& _IsOpen)
{
    if (!mPlanningUnit)
    {
        return;
    }

    if (ImGui::Begin("HTN Debugger Window", &_IsOpen, WindowFlags))
    {
        if (ImGui::BeginTabBar("Tab Bar", TabBarFlags))
        {
            if (ImGui::BeginTabItem("Active Plan"))
            {
                RenderActivePlan();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Database"))
            {
                RenderDatabase();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Decomposition"))
            {
                RenderDecomposition();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}

void HTNDebuggerWindow::RenderActivePlan()
{
}

void HTNDebuggerWindow::RenderDatabase()
{
    HTNWorldState& WorldState = mPlanningUnit->GetWorldStateMutable();

    ImGui::Text("Facts");

    ImGui::SameLine();

    const char* NewFactPopupName = "New Fact";
    if (ImGui::Button("+##AddFact"))
    {
        ImGui::OpenPopup(NewFactPopupName);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Add Fact");
    }

    static bool IsOpen = true;
    if (ImGui::BeginPopupModal(NewFactPopupName, &IsOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::string Name;
        ImGui::InputText("Name##Fact", &Name, InputTextFlags);

        static std::vector<HTNAtom> Arguments;
        ImGui::Text(std::format("Arguments ({})", Arguments.size()).c_str());

        ImGui::SameLine();

        if (ImGui::Button("+##AddArgument"))
        {
            Arguments.emplace_back(HTNAtom());
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Add Argument");
        }

        ImGui::SameLine();

        if (ImGui::Button("-##RemoveAllArguments"))
        {
            Arguments.clear();
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Remove All Arguments");
        }

        const char* Types[] = {"int", "float", "string"};

        for (size_t i = 0; i < Arguments.size(); ++i)
        {
            HTNAtom& Argument = Arguments[i];

            ImGui::Text(std::format("Argument {}", i).c_str());

            ImGui::SameLine();

            if (ImGui::Button(std::format("-##RemoveArgument{}", i).c_str()))
            {
                Arguments.erase(Arguments.begin() + i);
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Remove Argument");
            }

            int Type = 0;
            if (Argument.IsSet())
            {
                if (Argument.IsType<int>())
                {
                    Type = 0;
                }
                else if (Argument.IsType<float>())
                {
                    Type = 1;
                }
                else if (Argument.IsType<std::string>())
                {
                    Type = 2;
                }
            }

            ImGui::Combo(std::format("Type##{}", i).c_str(), &Type, Types, IM_ARRAYSIZE(Types));

            if (Type == 0)
            {
                int Value = (Argument.IsSet() && Argument.IsType<int>()) ? Argument.GetValue<int>() : 0;
                ImGui::InputInt(std::format("Value##{}", i).c_str(), &Value, 1, 100, InputTextFlags | ImGuiInputTextFlags_CharsDecimal);
                Argument = HTNAtom(Value);
            }
            else if (Type == 1)
            {
                float Value = (Argument.IsSet() && Argument.IsType<float>()) ? Argument.GetValue<float>() : 0.f;
                ImGui::InputFloat(std::format("Value##{}", i).c_str(), &Value, 0.f, 0.f, "%.2f", InputTextFlags | ImGuiInputTextFlags_CharsDecimal);
                Argument = HTNAtom(Value);
            }
            else if (Type == 2)
            {
                std::string Value = (Argument.IsSet() && Argument.IsType<std::string>()) ? Argument.GetValue<std::string>() : "";
                ImGui::InputText(std::format("Value##{}", i).c_str(), &Value, InputTextFlags);
                Argument = HTNAtom(Value);
            }
        }

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            WorldState.AddFact(Name.c_str(), Arguments);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("-##RemoveAllFacts"))
    {
        WorldState.RemoveAllFacts();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Remove All Facts");
    }

    static ImGuiTextFilter Filter;
    Filter.Draw();

    const std::unordered_map<std::string, HTNWorldState::HTNFact>& Facts = WorldState.GetFacts();
    for (auto It = Facts.begin(); It != Facts.end(); ++It)
    {
        const std::string&            Name   = It->first;
        const HTNWorldState::HTNFact& Tables = It->second;
        for (size_t i = 0; i < Tables.size(); ++i)
        {
            const HTNTableBase* Table = Tables[i];
            if (!Table)
            {
                continue;
            }

            const std::vector<std::vector<HTNAtom>> Entries = Table->GetEntries();
            for (size_t j = 0; j < Entries.size(); ++j)
            {
                const std::vector<HTNAtom>& Entry = Entries[j];

                std::string NameArguments = Name;
                for (const HTNAtom& Argument : Entry)
                {
                    NameArguments.append(" ");
                    NameArguments.append(Argument.ToString());
                }

                if (!Filter.PassFilter(NameArguments.c_str()))
                {
                    continue;
                }

                ImGui::Text(Name.c_str());

                for (const HTNAtom& Argument : Entry)
                {
                    ImGui::SameLine();
                    ImGui::Text(Argument.ToString().c_str());
                }

                ImGui::SameLine();

                if (ImGui::Button(std::format("-##RemoveFact{}{}", i, j).c_str()))
                {
                    WorldState.RemoveFact(Name.c_str(), static_cast<int>(i), static_cast<int>(j));
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Remove Fact");
                }
            }
        }
    }
}

void HTNDebuggerWindow::RenderDecomposition()
{
    // Parse domain
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

        static std::filesystem::path SelectedDomainPath;
        if (ImGui::BeginCombo("Domains", SelectedDomainPath.filename().stem().string().c_str(), ComboFlags))
        {
            for (const std::filesystem::path& DomainPath : DomainPaths)
            {
                const bool IsSelected = (SelectedDomainPath == DomainPath);
                if (ImGui::Selectable(DomainPath.filename().stem().string().c_str(), IsSelected, SelectableFlags))
                {
                    SelectedDomainPath = DomainPath;
                }

                if (IsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        enum ParseDomainResult : unsigned int
        {
            FAILED    = 0,
            SUCCEEDED = 1,
            NONE,
        };

        static ParseDomainResult LastParseDomainResult = ParseDomainResult::NONE;
        if (ImGui::Button("Parse Domain"))
        {
            LastParseDomainResult = static_cast<ParseDomainResult>(mPlanningUnit->ParseDomain(SelectedDomainPath.string()));
        }

        if (LastParseDomainResult == ParseDomainResult::FAILED)
        {
            ImGui::SameLine();
            ImGui::TextColored(FailColor, "FAILED");
        }
        else if (LastParseDomainResult == ParseDomainResult::SUCCEEDED)
        {
            ImGui::SameLine();
            ImGui::TextColored(SuccessColor, "SUCCEEDED");
        }
    }

    ImGui::Spacing();

    // Display AST
    {
        // TODO salvarez
    }
}
