#include "HTNDebuggerWindow.h"

#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/HTNInterpreter.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNPlannerHook.h"
#include "Runtime/HTNPlanningUnit.h"
#include "Runtime/HTNWorldState.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <filesystem>
#include <string>
#include <vector>

namespace
{
enum OperationResult : unsigned int;

void        RenderOperationResult(const OperationResult _OperationResult);
std::string RemoveDoubleQuotationMarks(const std::string& inString);

enum OperationResult : unsigned int
{
    FAILED    = 0,
    SUCCEEDED = 1,
    NONE,
};

const ImGuiWindowFlags     WindowFlags     = ImGuiWindowFlags_NoCollapse;
const ImGuiTabBarFlags     TabBarFlags     = ImGuiTabBarFlags_None;
const ImGuiComboFlags      ComboFlags      = ImGuiComboFlags_None;
const ImGuiSelectableFlags SelectableFlags = ImGuiSelectableFlags_None;
const ImGuiTableFlags      TableFlags      = ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders;
const ImGuiInputTextFlags  InputTextFlags  = ImGuiInputTextFlags_CharsNoBlank;

const ImVec4 FailColor    = ImVec4(1.f, 0.f, 0.f, 1.f);
const ImVec4 SuccessColor = ImVec4(0.f, 1.f, 0.f, 1.f);
} // namespace

namespace
{
void RenderOperationResult(const OperationResult _OperationResult)
{
    if (_OperationResult == OperationResult::FAILED)
    {
        ImGui::SameLine();
        ImGui::TextColored(FailColor, "FAILED");
    }
    else if (_OperationResult == OperationResult::SUCCEEDED)
    {
        ImGui::SameLine();
        ImGui::TextColored(SuccessColor, "SUCCEEDED");
    }
}

std::string RemoveDoubleQuotationMarks(const std::string& inString)
{
    return inString.substr(1, inString.size() - 2);
}
} // namespace

void HTNDebuggerWindow::Render(bool& _IsOpen)
{
    if (ImGui::Begin("HTN Debugger Window", &_IsOpen, WindowFlags))
    {
        if (ImGui::BeginTabBar("Tab Bar", TabBarFlags))
        {
            if (ImGui::BeginTabItem("Plan"))
            {
                RenderPlan();
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

void HTNDebuggerWindow::RenderPlan()
{
    // TODO salvarez Allow to plan all the planning units at the same time using multithreading. Display them like a map (check how Unreal does it),
    // not allowing to add an entry with the same key twice
    // TODO salvarez Display all of the resulting plans

    if (!mPlanner)
    {
        return;
    }

    static HTNPlanningUnit* SelectedPlanningUnit = nullptr;
    SelectPlanningUnit(SelectedPlanningUnit);

    if (!SelectedPlanningUnit)
    {
        return;
    }

    static std::string SelectedMethodName;
    if (ImGui::BeginCombo("Methods", SelectedMethodName.c_str(), ComboFlags))
    {
        const HTNInterpreter& Interpreter = mPlanner->GetInterpreter();
        if (const std::shared_ptr<const HTNDomain>& Domain = Interpreter.GetDomain())
        {
            const std::vector<std::shared_ptr<const HTNMethod>>& Methods = Domain->GetMethods();
            for (const std::shared_ptr<const HTNMethod>& Method : Methods)
            {
                if (!Method)
                {
                    continue;
                }

                const std::string MethodName = RemoveDoubleQuotationMarks(Method->GetName());
                const bool        IsSelected = (SelectedMethodName == MethodName);
                if (ImGui::Selectable(MethodName.c_str(), IsSelected, SelectableFlags))
                {
                    SelectedMethodName = MethodName;
                }

                if (IsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }

        ImGui::EndCombo();
    }

    static std::vector<HTNTaskInstance> LastPlan;
    static OperationResult              LastPlanResult   = OperationResult::NONE;
    static HTNPlanningUnit*             LastPlanningUnit = nullptr;
    static std::string                  LastMethodName;
    if (ImGui::Button("Plan"))
    {
        LastPlan         = SelectedPlanningUnit->ExecuteTopLevelMethod(SelectedMethodName);
        LastPlanResult   = static_cast<OperationResult>(!LastPlan.empty());
        LastPlanningUnit = SelectedPlanningUnit;
        LastMethodName   = SelectedMethodName;
    }

    RenderOperationResult(LastPlanResult);

    if (LastPlanResult != OperationResult::SUCCEEDED)
    {
        return;
    }

    ImGui::Text(LastPlanningUnit->GetName().c_str());
    ImGui::SameLine();
    ImGui::Text(LastMethodName.c_str());

    for (const HTNTaskInstance& Task : LastPlan)
    {
        const std::string& TaskName = RemoveDoubleQuotationMarks(Task.GetName());
        ImGui::Text(TaskName.c_str());

        const std::vector<HTNAtom>& Arguments = Task.GetArguments();
        for (const HTNAtom& Argument : Arguments)
        {
            const std::string& ArgumentName = Argument.ToString();
            ImGui::SameLine();
            ImGui::Text(ArgumentName.c_str());
        }
    }
}

void HTNDebuggerWindow::RenderDatabase()
{
    static HTNPlanningUnit* SelectedPlanningUnit = nullptr;
    SelectPlanningUnit(SelectedPlanningUnit);

    if (!SelectedPlanningUnit)
    {
        return;
    }

    HTNWorldState& WorldState = SelectedPlanningUnit->GetWorldStateMutable();

    ImGui::Text("Facts");

    ImGui::SameLine();

    const char* NewFactPopupName = "New Fact";
    if (ImGui::Button("+"))
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
        ImGui::InputText("Name", &Name, InputTextFlags);

        static std::vector<HTNAtom> Arguments;
        ImGui::Text("Arguments");

        ImGui::SameLine();

        if (ImGui::Button("+"))
        {
            Arguments.emplace_back(HTNAtom());
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Add Argument");
        }

        ImGui::SameLine();

        if (ImGui::Button("-"))
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

            ImGui::Combo(std::format("##Type{}", i).c_str(), &Type, Types, IM_ARRAYSIZE(Types));

            ImGui::SameLine();

            if (Type == 0)
            {
                int Value = (Argument.IsSet() && Argument.IsType<int>()) ? Argument.GetValue<int>() : 0;
                ImGui::InputInt(std::format("##Value{}", i).c_str(), &Value, 0, 0, InputTextFlags | ImGuiInputTextFlags_CharsDecimal);
                Argument = HTNAtom(Value);
            }
            else if (Type == 1)
            {
                float Value = (Argument.IsSet() && Argument.IsType<float>()) ? Argument.GetValue<float>() : 0.f;
                ImGui::InputFloat(std::format("##Value{}", i).c_str(), &Value, 0.f, 0.f, "%.2f", InputTextFlags | ImGuiInputTextFlags_CharsDecimal);
                Argument = HTNAtom(Value);
            }
            else if (Type == 2)
            {
                std::string Value = (Argument.IsSet() && Argument.IsType<std::string>()) ? Argument.GetValue<std::string>() : "";
                ImGui::InputText(std::format("##Value{}", i).c_str(), &Value, InputTextFlags);
                Argument = HTNAtom(Value);
            }

            ImGui::SameLine();

            if (ImGui::Button(std::format("-##{}", i).c_str()))
            {
                Arguments.erase(Arguments.begin() + i);
            }

            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Remove Argument");
            }
        }

        if (ImGui::Button("OK", ImVec2(120.f, 0.f)))
        {
            WorldState.AddFact(Name.c_str(), Arguments);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120.f, 0.f)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("-"))
    {
        WorldState.RemoveAllFacts();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Remove All Facts");
    }

    static ImGuiTextFilter Filter;
    Filter.Draw("##");

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

                if (ImGui::Button(std::format("-##{}{}{}", Name, i, j).c_str()))
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
    if (!mPlanner)
    {
        return;
    }

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

    static OperationResult LastParseResult = OperationResult::NONE;
    if (ImGui::Button("Parse"))
    {
        LastParseResult = static_cast<OperationResult>(mPlanner->ParseDomain(SelectedDomainPath.string()));
    }

    RenderOperationResult(LastParseResult);

    if (LastParseResult != OperationResult::SUCCEEDED)
    {
        return;
    }

    // TODO salvarez Draw AST
    // const HTNInterpreter&                   Interpreter = mPlanner->GetInterpreter();
    // const std::shared_ptr<const HTNDomain>& Domain      = Interpreter.GetDomain();
}

void HTNDebuggerWindow::SelectPlanningUnit(HTNPlanningUnit*& inSelectedPlanningUnit)
{
    if (ImGui::BeginCombo("Planning Unit", inSelectedPlanningUnit ? inSelectedPlanningUnit->GetName().c_str() : "", ComboFlags))
    {
        for (HTNPlanningUnit& PlanningUnit : mPlanningUnits)
        {
            const bool IsSelected = (inSelectedPlanningUnit == &PlanningUnit);
            if (ImGui::Selectable(PlanningUnit.GetName().c_str(), IsSelected, SelectableFlags))
            {
                // TODO salvarez This works assuming that the array will never change
                inSelectedPlanningUnit = &PlanningUnit;
            }

            if (IsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
}
