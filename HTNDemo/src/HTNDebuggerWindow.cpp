#include "HTNDebuggerWindow.h"

#include "Domain/AST/HTNAxiomNode.h"
#include "Domain/AST/HTNBranchNode.h"
#include "Domain/AST/HTNConditionNode.h"
#include "Domain/AST/HTNDomainNode.h"
#include "Domain/AST/HTNMethodNode.h"
#include "Domain/AST/HTNTaskNode.h"
#include "Domain/HTNDomainPrinter.h"
#include "HTNAtom.h"
#include "HTNPlanningUnit.h"
#include "Interpreter/HTNInterpreter.h"
#include "Interpreter/HTNTaskResult.h"
#include "Planner/HTNPlannerHook.h"
#include "WorldState/HTNWorldStateHook.h"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <algorithm>
#include <execution>
#include <filesystem>
#include <string>
#include <format>
#include <vector>

namespace
{
enum OperationResult : unsigned int;

void RenderFileSelector(const std::string& inDirectoryName, const std::string& inFileExtension, std::filesystem::path& ioSelectedFilePath);
void RenderOperationResult(const OperationResult inOperationResult);

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
void RenderFileSelector(const std::string& inDirectoryName, const std::string& inFileExtension, std::filesystem::path& ioSelectedFilePath)
{
    std::vector<std::filesystem::path> FilePaths;

    const std::filesystem::path DirectoryPath = std::filesystem::current_path().parent_path() / std::filesystem::path(inDirectoryName);
    for (const std::filesystem::directory_entry& DirectoryEntry : std::filesystem::directory_iterator(DirectoryPath))
    {
        if (!DirectoryEntry.is_regular_file())
        {
            continue;
        }

        const std::filesystem::path FilePath = DirectoryEntry.path();
        if (FilePath.extension() != inFileExtension)
        {
            continue;
        }

        FilePaths.emplace_back(FilePath);
    }

    if (ImGui::BeginCombo("File", ioSelectedFilePath.filename().stem().string().c_str(), ComboFlags))
    {
        for (const std::filesystem::path& FilePath : FilePaths)
        {
            const bool IsFileSelected = (ioSelectedFilePath == FilePath);
            if (ImGui::Selectable(FilePath.filename().stem().string().c_str(), IsFileSelected, SelectableFlags))
            {
                ioSelectedFilePath = FilePath;
            }

            if (IsFileSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
}

void RenderOperationResult(const OperationResult inOperationResult)
{
    if (inOperationResult == OperationResult::FAILED)
    {
        ImGui::SameLine();
        ImGui::TextColored(FailColor, "FAILED");
    }
    else if (inOperationResult == OperationResult::SUCCEEDED)
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

            if (ImGui::BeginTabItem("Plan"))
            {
                RenderPlan();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Domain"))
            {
                RenderDomain();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("World State"))
            {
                RenderWorldState();
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

void HTNDebuggerWindow::RenderPlan()
{
    const std::shared_ptr<const HTNDomainNode>&              DomainNode  = mPlannerHook->GetDomainNode();
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
    static OperationResult       LastPlanResult = OperationResult::NONE;
    if (ImGui::Button("Plan"))
    {
        LastPlans.clear();

        std::for_each(std::execution::par, EntryPoints.begin(), EntryPoints.end(), [this](EntryPoint& inEntryPoint) {
            for (unsigned int i = 0; i < inEntryPoint.Amount; ++i)
            {
                const HTNPlanningUnit      PlanningUnit = HTNPlanningUnit(*mPlannerHook, *mWorldStateHook);
                std::vector<HTNTaskResult> Plan;
                if (!PlanningUnit.ExecuteTopLevelMethod(inEntryPoint.ID, Plan))
                {
                    continue;
                }

                LastPlans.emplace_back(inEntryPoint.ID, Plan);
            }
        });

        LastPlanResult = static_cast<OperationResult>(!LastPlans.empty());
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Plan the selected entry points of the parsed domain using the parsed world state");
    }

    RenderOperationResult(LastPlanResult);

    ImGui::Separator();

    for (const LastPlan& LastPlan : LastPlans)
    {
        ImGui::Text(LastPlan.EntryPointID.c_str());

        ImGui::Indent();
        for (const HTNTaskResult& Task : LastPlan.Plan)
        {
            ImGui::Text(Task.GetID().c_str());

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

void HTNDebuggerWindow::RenderDomain()
{
    static const std::string     DomainsDirectoryName = "Domains";
    static const std::string     DomainFileExtension  = ".domain";
    static std::filesystem::path SelectedDomainFilePath;
    RenderFileSelector(DomainsDirectoryName, DomainFileExtension, SelectedDomainFilePath);

    static OperationResult LastParseDomainResult = OperationResult::NONE;
    if (ImGui::Button("Parse"))
    {
        LastParseDomainResult = static_cast<OperationResult>(mPlannerHook->ParseDomainFile(SelectedDomainFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected domain file");
    }

    RenderOperationResult(LastParseDomainResult);

    ImGui::Separator();

    const std::shared_ptr<const HTNDomainNode>& DomainNode = mPlannerHook->GetDomainNode();
    if (!DomainNode)
    {
        return;
    }

    HTNDomainPrinter DomainPrinter = HTNDomainPrinter(DomainNode);
    DomainPrinter.Print();
}

void HTNDebuggerWindow::RenderWorldState()
{
    static const std::string     WorldStatesDirectoryName = "WorldStates";
    static const std::string     WorldStateFileExtension  = ".worldstate";
    static std::filesystem::path SelectedWorldStateFilePath;
    RenderFileSelector(WorldStatesDirectoryName, WorldStateFileExtension, SelectedWorldStateFilePath);

    static OperationResult LastImportWorldStateResult = OperationResult::NONE;
    if (ImGui::Button("Parse"))
    {
        LastImportWorldStateResult = static_cast<OperationResult>(mWorldStateHook->ImportWorldStateFile(SelectedWorldStateFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected world state file");
    }

    RenderOperationResult(LastImportWorldStateResult);

    ImGui::Separator();

    static ImGuiTextFilter Filter;
    Filter.Draw("##");

    const HTNWorldState&                                           WorldState = mWorldStateHook->GetWorldState();
    const std::unordered_map<std::string, HTNWorldState::HTNFact>& Facts      = WorldState.GetFacts();
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

                    std::string ArgumentString = Argument.ToString();
                    if (Argument.IsType<std::string>())
                    {
                        ArgumentString = std::format("\"{}\"", ArgumentString);
                    }

                    ImGui::Text(ArgumentString.c_str());
                }
            }
        }
    }
}
