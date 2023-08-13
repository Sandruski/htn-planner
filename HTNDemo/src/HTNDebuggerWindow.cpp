#include "HTNDebuggerWindow.h"

#include "Interpreter/AST/HTNBranch.h"
#include "Interpreter/AST/HTNCondition.h"
#include "Interpreter/AST/HTNDomain.h"
#include "Interpreter/AST/HTNMethod.h"
#include "Interpreter/AST/HTNTask.h"
#include "Interpreter/HTNInterpreter.h"
#include "Runtime/HTNAtom.h"
#include "Runtime/HTNPlannerHook.h"
#include "Runtime/HTNPlanningUnit.h"
#include "Runtime/HTNWorldState.h"
#include "imgui.h"
#include "imgui_stdlib.h"

#include <algorithm>
#include <execution>
#include <filesystem>
#include <string>
#include <vector>

namespace
{
enum OperationResult : unsigned int;

void RenderOperationResult(const OperationResult _OperationResult);

enum OperationResult : unsigned int
{
    FAILED    = 0,
    SUCCEEDED = 1,
    NONE,
};

const ImGuiWindowFlags     WindowFlags     = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
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
    if (!mPlanner)
    {
        return;
    }

    if (!mWorldState)
    {
        return;
    }

    struct EntryPoint
    {
        std::string  Name;
        unsigned int Amount = 0;
    };

    static std::vector<EntryPoint> EntryPoints;
    ImGui::Text("Entry Points");

    ImGui::SameLine();

    if (ImGui::Button("+"))
    {
        EntryPoints.emplace_back();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Add Entry Point");
    }

    ImGui::SameLine();

    if (ImGui::Button("-"))
    {
        EntryPoints.clear();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Remove All Entry Points");
    }

    const HTNInterpreter&                                Interpreter = mPlanner->GetInterpreter();
    const std::shared_ptr<const HTNDomain>&              Domain      = Interpreter.GetDomain();
    const std::vector<std::shared_ptr<const HTNMethod>>* Methods     = Domain ? &Domain->GetMethods() : nullptr;

    for (size_t i = 0; i < EntryPoints.size(); ++i)
    {
        EntryPoint& EntryPoint = EntryPoints[i];

        if (ImGui::BeginCombo(std::format("##Method{}", i).c_str(), EntryPoint.Name.c_str(), ComboFlags))
        {
            if (Methods)
            {
                for (const std::shared_ptr<const HTNMethod>& Method : *Methods)
                {
                    if (!Method)
                    {
                        continue;
                    }

                    const std::string MethodName = Method->GetName();
                    const bool        IsSelected = (EntryPoint.Name == MethodName);
                    if (ImGui::Selectable(MethodName.c_str(), IsSelected, SelectableFlags))
                    {
                        EntryPoint.Name = MethodName;
                    }

                    if (IsSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }

            ImGui::EndCombo();
        }

        ImGui::SameLine();

        int Amount = EntryPoint.Amount;
        if (ImGui::InputInt(std::format("##Amount{}", i).c_str(), &Amount, 0, 0, InputTextFlags | ImGuiInputTextFlags_CharsDecimal))
        {
            EntryPoint.Amount = std::max(0, Amount);
        }

        ImGui::SameLine();

        if (ImGui::Button(std::format("-##{}", i).c_str()))
        {
            EntryPoints.erase(EntryPoints.begin() + i);
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("Remove Entry Point");
        }
    }

    struct LastPlan
    {
        std::string                  EntryPointName;
        std::vector<HTNTaskInstance> Plan;
    };

    static std::vector<LastPlan> LastPlans;
    static OperationResult       LastPlanResult = OperationResult::NONE;
    if (ImGui::Button("Plan"))
    {
        LastPlans.clear();

        std::for_each(std::execution::par, EntryPoints.begin(), EntryPoints.end(), [this](EntryPoint& inEntryPoint) {
            for (unsigned int i = 0; i < inEntryPoint.Amount; ++i)
            {
                const HTNPlanningUnit               PlanningUnit = HTNPlanningUnit(*mPlanner, *mWorldState);
                const std::vector<HTNTaskInstance>& Plan         = PlanningUnit.ExecuteTopLevelMethod(inEntryPoint.Name);
                if (Plan.empty())
                {
                    continue;
                }

                LastPlans.emplace_back(inEntryPoint.Name, Plan);
            }
        });

        LastPlanResult = static_cast<OperationResult>(!LastPlans.empty());
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry points of the domain using the world state");
    }

    RenderOperationResult(LastPlanResult);

    if (LastPlanResult != OperationResult::SUCCEEDED)
    {
        return;
    }

    for (const LastPlan& LastPlan : LastPlans)
    {
        ImGui::Text(LastPlan.EntryPointName.c_str());

        ImGui::Indent();
        for (const HTNTaskInstance& Task : LastPlan.Plan)
        {
            const std::string& TaskName = Task.GetName();
            ImGui::Text(TaskName.c_str());

            const std::vector<HTNAtom>& Arguments = Task.GetArguments();
            for (const HTNAtom& Argument : Arguments)
            {
                const std::string& ArgumentName = Argument.ToString(false);
                ImGui::SameLine();
                ImGui::Text(ArgumentName.c_str());
            }
        }
        ImGui::Unindent();
    }
}

void HTNDebuggerWindow::RenderDatabase()
{
    if (!mWorldState)
    {
        return;
    }

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

    if (ImGui::BeginPopupModal(NewFactPopupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::string Name;
        ImGui::InputText("Name", &Name, InputTextFlags);

        static std::vector<HTNAtom> Arguments;
        ImGui::Text("Arguments");

        ImGui::SameLine();

        if (ImGui::Button("+"))
        {
            Arguments.emplace_back();
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

        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(120.f, 0.f)))
        {
            mWorldState->AddFact(Name.c_str(), Arguments);
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
        mWorldState->RemoveAllFacts();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Remove All Facts");
    }

    static ImGuiTextFilter Filter;
    Filter.Draw("##");

    const std::unordered_map<std::string, HTNWorldState::HTNFact>& Facts = mWorldState->GetFacts();
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
                    NameArguments.append(std::format(" {}", Argument.ToString(false)));
                }

                if (!Filter.PassFilter(NameArguments.c_str()))
                {
                    continue;
                }

                ImGui::Text(Name.c_str());

                for (const HTNAtom& Argument : Entry)
                {
                    ImGui::SameLine();
                    ImGui::Text(Argument.ToString(true).c_str());
                }

                ImGui::SameLine();

                if (ImGui::Button(std::format("-##{}{}{}", Name, i, j).c_str()))
                {
                    mWorldState->RemoveFact(Name.c_str(), static_cast<int>(i), static_cast<int>(j));
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
    if (ImGui::BeginCombo("Domain", SelectedDomainPath.filename().stem().string().c_str(), ComboFlags))
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

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected domain");
    }

    RenderOperationResult(LastParseResult);

    if (LastParseResult != OperationResult::SUCCEEDED)
    {
        return;
    }

    const HTNInterpreter& Interpreter = mPlanner->GetInterpreter();

    const std::shared_ptr<const HTNDomain>& Domain = Interpreter.GetDomain();
    ImGui::Text(Domain->ToString().c_str());

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNMethod>>& Methods = Domain->GetMethods();
    for (const std::shared_ptr<const HTNMethod>& Method : Methods)
    {
        ImGui::Text(Method->ToString().c_str());

        ImGui::Indent();
        const std::vector<std::shared_ptr<const HTNBranch>>& Branches = Method->GetBranches();
        for (const std::shared_ptr<const HTNBranch>& Branch : Branches)
        {
            ImGui::Text(Branch->ToString().c_str());

            ImGui::Indent();
            const std::shared_ptr<const HTNConditionBase>& PreCondition = Branch->GetPreCondition();
            if (PreCondition)
            {
                ImGui::Text(PreCondition->ToString().c_str());
            }

            const std::vector<std::shared_ptr<const HTNTask>>& Tasks = Branch->GetTasks();
            for (const std::shared_ptr<const HTNTask>& Task : Tasks)
            {
                ImGui::Text(Task->ToString().c_str());
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();
    }
    ImGui::Unindent();
}
