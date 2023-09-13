#include "HTNDebuggerWindow.h"

#include "Database/HTNWorldState.h"
#include "HTNAtom.h"
#include "HTNPlanningUnit.h"
#include "Interpreter/HTNInterpreter.h"
#include "Parser/AST/HTNAxiomNode.h"
#include "Parser/AST/HTNBranchNode.h"
#include "Parser/AST/HTNConditionNode.h"
#include "Parser/AST/HTNDomainNode.h"
#include "Parser/AST/HTNMethodNode.h"
#include "Parser/AST/HTNTaskNode.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNTaskResult.h"

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
            if (ImGui::BeginTabItem("Active Plan"))
            {
                RenderActivePlan();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Decomposition"))
            {
                RenderDecomposition();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Parsing"))
            {
                RenderParsing();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Database"))
            {
                RenderDatabase();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}

void HTNDebuggerWindow::RenderActivePlan()
{
    // TODO salvarez
}

void HTNDebuggerWindow::RenderDecomposition()
{
    if (!mPlanner)
    {
        return;
    }

    if (!mWorldState)
    {
        return;
    }

    const HTNInterpreter&                                    Interpreter = mPlanner->GetInterpreter();
    const std::shared_ptr<const HTNDomainNode>&              DomainNode  = Interpreter.GetDomainNode();
    const std::vector<std::shared_ptr<const HTNMethodNode>>* MethodNodes = nullptr;
    if (DomainNode)
    {
        if (DomainNode->IsTopLevel())
        {
            MethodNodes = &DomainNode->GetMethodNodes();
        }
    }

    struct EntryPoint
    {
        std::string  ID;
        unsigned int Amount = 0;
    };

    static std::vector<EntryPoint> EntryPoints;
    ImGui::Text("Entry Points");

    ImGui::SameLine();

    if (ImGui::Button("+"))
    {
        const std::string      DefaultEntryPointID     = (MethodNodes && !MethodNodes->empty()) ? (*MethodNodes)[0]->GetID() : "";
        constexpr unsigned int DefaultEntryPointAmount = 1;
        EntryPoints.emplace_back(DefaultEntryPointID, DefaultEntryPointAmount);
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

    for (size_t i = 0; i < EntryPoints.size(); ++i)
    {
        EntryPoint& EntryPoint = EntryPoints[i];

        if (!EntryPoint.ID.empty())
        {
            if (MethodNodes)
            {
                const auto MethodNode =
                    std::find_if(MethodNodes->begin(), MethodNodes->end(), [&EntryPoint](const std::shared_ptr<const HTNMethodNode>& inMethodNode) {
                        if (!inMethodNode)
                        {
                            return false;
                        }

                        if (!inMethodNode->IsTopLevel())
                        {
                            return false;
                        }

                        return (EntryPoint.ID == inMethodNode->GetID());
                    });

                if (MethodNode == MethodNodes->end())
                {
                    EntryPoint.ID.clear();
                }
            }
            else
            {
                EntryPoint.ID.clear();
            }
        }

        if (ImGui::BeginCombo(std::format("##Method{}", i).c_str(), EntryPoint.ID.c_str(), ComboFlags))
        {
            if (MethodNodes)
            {
                for (const std::shared_ptr<const HTNMethodNode>& MethodNode : *MethodNodes)
                {
                    if (!MethodNode)
                    {
                        continue;
                    }

                    if (!MethodNode->IsTopLevel())
                    {
                        continue;
                    }

                    const std::string MethodNodeID = MethodNode->GetID();
                    const bool        IsSelected   = (EntryPoint.ID == MethodNodeID);
                    if (ImGui::Selectable(MethodNodeID.c_str(), IsSelected, SelectableFlags))
                    {
                        EntryPoint.ID = MethodNodeID;
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
        std::string                EntryPointID;
        std::vector<HTNTaskResult> Plan;
    };

    static std::vector<LastPlan> LastPlans;
    static OperationResult       LastDecompositionResult = OperationResult::NONE;
    if (ImGui::Button("Decompose"))
    {
        LastPlans.clear();

        std::for_each(std::execution::par, EntryPoints.begin(), EntryPoints.end(), [this](EntryPoint& inEntryPoint) {
            for (unsigned int i = 0; i < inEntryPoint.Amount; ++i)
            {
                const HTNPlanningUnit             PlanningUnit = HTNPlanningUnit(*mPlanner, *mWorldState);
                const std::vector<HTNTaskResult>& Plan         = PlanningUnit.ExecuteTopLevelMethod(inEntryPoint.ID);
                if (Plan.empty())
                {
                    continue;
                }

                LastPlans.emplace_back(inEntryPoint.ID, Plan);
            }
        });

        LastDecompositionResult = static_cast<OperationResult>(!LastPlans.empty());
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry points of the domain using the database");
    }

    RenderOperationResult(LastDecompositionResult);

    if (LastDecompositionResult != OperationResult::SUCCEEDED)
    {
        return;
    }

    for (const LastPlan& LastPlan : LastPlans)
    {
        ImGui::Text(LastPlan.EntryPointID.c_str());

        ImGui::Indent();
        for (const HTNTaskResult& Task : LastPlan.Plan)
        {
            ImGui::Text(Task.GetName().c_str());

            const std::vector<HTNAtom>& Arguments = Task.GetArguments();
            for (const HTNAtom& Argument : Arguments)
            {
                ImGui::SameLine();
                ImGui::Text(Argument.ToString().c_str());
            }
        }
        ImGui::Unindent();
    }
}

void HTNDebuggerWindow::RenderParsing()
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

    static std::string     LastDomainPath;
    static OperationResult LastParsingResult = OperationResult::NONE;
    if (ImGui::Button("Parse"))
    {
        LastDomainPath    = SelectedDomainPath.filename().stem().string();
        LastParsingResult = static_cast<OperationResult>(mPlanner->ParseDomain(SelectedDomainPath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected domain");
    }

    RenderOperationResult(LastParsingResult);

    if (LastParsingResult != OperationResult::SUCCEEDED)
    {
        return;
    }

    ImGui::Text(LastDomainPath.c_str());

    // TODO salvarez Do this in the Printer class
    ImGui::Indent();
    const HTNInterpreter&                       Interpreter = mPlanner->GetInterpreter();
    const std::shared_ptr<const HTNDomainNode>& DomainNode  = Interpreter.GetDomainNode();
    ImGui::Text(DomainNode->ToString().c_str());

    ImGui::Indent();
    const std::vector<std::shared_ptr<const HTNAxiomNode>>& AxiomNodes = DomainNode->GetAxiomNodes();
    for (const std::shared_ptr<const HTNAxiomNode>& AxiomNode : AxiomNodes)
    {
        ImGui::Text(AxiomNode->ToString().c_str());

        ImGui::Indent();
        if (const std::shared_ptr<const HTNConditionNodeBase>& ConditionNode = AxiomNode->GetConditionNode())
        {
            ImGui::Text(ConditionNode->ToString().c_str());
        }
        ImGui::Unindent();
    }

    const std::vector<std::shared_ptr<const HTNMethodNode>>& MethodNodes = DomainNode->GetMethodNodes();
    for (const std::shared_ptr<const HTNMethodNode>& MethodNode : MethodNodes)
    {
        ImGui::Text(MethodNode->ToString().c_str());

        ImGui::Indent();
        const std::vector<std::shared_ptr<const HTNBranchNode>>& BranchNodes = MethodNode->GetBranchNodes();
        for (const std::shared_ptr<const HTNBranchNode>& BranchNode : BranchNodes)
        {
            ImGui::Text(BranchNode->ToString().c_str());

            ImGui::Indent();
            if (const std::shared_ptr<const HTNConditionNodeBase>& PreConditionNode = BranchNode->GetPreConditionNode())
            {
                ImGui::Text(PreConditionNode->ToString().c_str());
            }

            const std::vector<std::shared_ptr<const HTNTaskNode>>& TaskNodes = BranchNode->GetTaskNodes();
            for (const std::shared_ptr<const HTNTaskNode>& TaskNode : TaskNodes)
            {
                ImGui::Text(TaskNode->ToString().c_str());
            }
            ImGui::Unindent();
        }
        ImGui::Unindent();
    }
    ImGui::Unindent();
    ImGui::Unindent();
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
                    NameArguments.append(std::format(" {}", Argument.ToString()));
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
