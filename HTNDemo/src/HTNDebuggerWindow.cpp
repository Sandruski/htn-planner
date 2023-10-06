#include "HTNDebuggerWindow.h"

#include "Domain/HTNDecompositionPrinter.h"
#include "Domain/HTNDomainPrinter.h"
#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Interpreter/HTNTaskResult.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "HTNAtom.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNPlanningUnit.h"
#include "WorldState/HTNWorldStatePrinter.h"

#include "imgui.h"
#include "imgui_stdlib.h"

#include <algorithm>
#include <execution>
#include <filesystem>
#include <format>
#include <mutex>
#include <string>
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
            if (ImGui::BeginTabItem("Plan"))
            {
                RenderPlan();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Decomposition"))
            {
                RenderDecomposition();
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

void HTNDebuggerWindow::RenderPlan()
{
    // TODO salvarez
}

void HTNDebuggerWindow::RenderDecomposition()
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

    struct HTNEntryPoint
    {
        std::string  mID;
        unsigned int mAmount = 0;
    };

    static std::vector<HTNEntryPoint> EntryPoints;
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

    for (std::size_t i = 0; i < EntryPoints.size(); ++i)
    {
        HTNEntryPoint& EntryPoint = EntryPoints[i];

        if (!EntryPoint.mID.empty())
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

                        return (EntryPoint.mID == inMethodNode->GetID());
                    });

                if (MethodNode == MethodNodes->end())
                {
                    EntryPoint.mID.clear();
                }
            }
            else
            {
                EntryPoint.mID.clear();
            }
        }

        if (ImGui::BeginCombo(std::format("##Method{}", i).c_str(), EntryPoint.mID.c_str(), ComboFlags))
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
                    const bool        IsSelected   = (EntryPoint.mID == MethodNodeID);
                    if (ImGui::Selectable(MethodNodeID.c_str(), IsSelected, SelectableFlags))
                    {
                        EntryPoint.mID = MethodNodeID;
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

        int Amount = EntryPoint.mAmount;
        if (ImGui::InputInt(std::format("##Amount{}", i).c_str(), &Amount, 0, 0, InputTextFlags | ImGuiInputTextFlags_CharsDecimal))
        {
            EntryPoint.mAmount = std::max(0, Amount);
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

    struct HTNDecomposition
    {
        std::string                          mEntryPointID;
        std::shared_ptr<const HTNDomainNode> mDomainNode;
        HTNDecompositionContext              mDecompositionContext;
    };

    static std::mutex                    LastDecompositionsMutex;
    static std::vector<HTNDecomposition> LastDecompositions;
    static OperationResult               LastDecompositionResult = OperationResult::NONE;
    if (ImGui::Button("Decompose"))
    {
        LastDecompositionsMutex.lock();
        LastDecompositions.clear();
        LastDecompositionsMutex.unlock();

        std::for_each(std::execution::par, EntryPoints.begin(), EntryPoints.end(), [this](HTNEntryPoint& inEntryPoint) {
            for (unsigned int i = 0; i < inEntryPoint.mAmount; ++i)
            {
                // AI has upper-body planning unit and lower-body planning unit
                HTNPlanningUnit PlanningUnit = HTNPlanningUnit(*mPlannerHook, *mDatabaseHook);
                if (!PlanningUnit.ExecuteTopLevelMethod(inEntryPoint.mID))
                {
                    continue;
                }

                // TODO salvarez Just add it once per entry point since it should be the same result
                LastDecompositionsMutex.lock();
                const std::shared_ptr<const HTNDomainNode>& LastDomainNode           = mPlannerHook->GetDomainNode();
                const HTNDecompositionContext&              LastDecompositionContext = PlanningUnit.GetLastDecompositionContext();
                LastDecompositions.emplace_back(inEntryPoint.mID, LastDomainNode, LastDecompositionContext);
                LastDecompositionsMutex.unlock();
            }
        });

        LastDecompositionResult = static_cast<OperationResult>(!LastDecompositions.empty());
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry points of the parsed domain using the parsed world state");
    }

    RenderOperationResult(LastDecompositionResult);

    ImGui::Separator();

    LastDecompositionsMutex.lock();
    for (const HTNDecomposition& LastDecomposition : LastDecompositions)
    {
        const HTNDecompositionSnapshotDebug& LastDecompositionSnapshot = LastDecomposition.mDecompositionContext.GetDecompositionSnapshot();
        HTNDecompositionPrinter              DecompositionPrinter =
            HTNDecompositionPrinter(LastDecomposition.mDomainNode, LastDecomposition.mEntryPointID, LastDecompositionSnapshot);
        DecompositionPrinter.Print();
    }
    // TODO salvarez Print last plan tasks in Plan window
    /*
    for (const HTNDecomposition& LastDecomposition : LastDecompositions)
    {
        ImGui::Text(LastDecomposition.mEntryPointID.c_str());

        ImGui::Indent();
        const std::vector<HTNTaskResult>&  LastPlan = LastDecomposition.mDecompositionContext.GetCurrentDecomposition().GetPlan();
        for (const HTNTaskResult& Task : LastPlan)
        {
            ImGui::Text(Task.GetID().c_str());

            const std::vector<HTNAtom>& Arguments = Task.GetArguments();
            for (const HTNAtom& Argument : Arguments)
            {
                ImGui::SameLine();
                static constexpr bool ShouldDoubleQuoteString = true;
                ImGui::Text(Argument.ToString(ShouldDoubleQuoteString).c_str());
            }
        }
        ImGui::Unindent();
    }
    */
    LastDecompositionsMutex.unlock();
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

    static HTNDomainPrinter DomainPrinter;
    DomainPrinter.Print(DomainNode);
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
        LastImportWorldStateResult = static_cast<OperationResult>(mDatabaseHook->ParseWorldStateFile(SelectedWorldStateFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected world state file");
    }

    RenderOperationResult(LastImportWorldStateResult);

    ImGui::Separator();

    static ImGuiTextFilter TextFilter;
    TextFilter.Draw("##");

    static const HTNWorldStatePrinter WorldStatePrinter;
    const HTNWorldState&              WorldState = mDatabaseHook->GetWorldState();
    WorldStatePrinter.Print(WorldState, TextFilter);
}
