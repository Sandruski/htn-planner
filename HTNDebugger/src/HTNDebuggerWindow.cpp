#include "HTNDebuggerWindow.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionPrinter.h"
#include "Domain/HTNDecompositionWatchWindowPrinter.h"
#include "Domain/HTNDomainPrinter.h"
#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Interpreter/HTNNodePath.h"
#include "Domain/Interpreter/HTNTaskResult.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "HTNImGuiHelpers.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNPlanningUnit.h"
#include "WorldState/HTNWorldStatePrinter.h"

#include "imgui.h"

#include <execution>
#include <filesystem>
#include <mutex>

namespace
{
enum HTNOperationResult : unsigned int
{
    FAILED    = 0,
    SUCCEEDED = 1,
    NONE,
};

struct HTNPlanningQuery
{
    explicit HTNPlanningQuery(HTNPlanningUnit* inPlanningUnit) : mPlanningUnit(inPlanningUnit)
    {
    }

    HTNPlanningUnit*   mPlanningUnit = nullptr;
    std::string        mEntryPointID;
    HTNOperationResult mLastDecompositionResult = HTNOperationResult::NONE;
};

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

    if (ImGui::BeginCombo("File", ioSelectedFilePath.filename().stem().string().c_str(), HTNImGuiHelpers::kDefaultComboFlags))
    {
        for (const std::filesystem::path& FilePath : FilePaths)
        {
            const bool IsFileSelected = (ioSelectedFilePath == FilePath);
            if (ImGui::Selectable(FilePath.filename().stem().string().c_str(), IsFileSelected, HTNImGuiHelpers::kDefaultSelectableFlags))
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

void RenderOperationResult(const HTNOperationResult inOperationResult)
{
    if (inOperationResult == HTNOperationResult::FAILED)
    {
        const bool   Result = static_cast<bool>(inOperationResult);
        const ImVec4 Color  = HTNImGuiHelpers::GetResultColor(Result);
        ImGui::SameLine();
        ImGui::TextColored(Color, "FAILED");
    }
    else if (inOperationResult == HTNOperationResult::SUCCEEDED)
    {
        const bool   Result = static_cast<bool>(inOperationResult);
        const ImVec4 Color  = HTNImGuiHelpers::GetResultColor(Result);
        ImGui::SameLine();
        ImGui::TextColored(Color, "SUCCEEDED");
    }
}

void RenderActivePlanByPlanningUnit(const HTNPlanningUnit& inPlanningUnit)
{
    const HTNDecompositionContext&    LastDecompositionContext = inPlanningUnit.GetLastDecompositionContext();
    const HTNDecompositionRecord&     CurrentDecomposition     = LastDecompositionContext.GetCurrentDecomposition();
    const std::vector<HTNTaskResult>& Plan                     = CurrentDecomposition.GetPlan();
    for (const HTNTaskResult& TaskResult : Plan)
    {
        const std::string& TaskID = TaskResult.GetID();
        ImGui::Text(TaskID.c_str());

        const std::vector<HTNAtom>& Arguments = TaskResult.GetArguments();
        for (const HTNAtom& Argument : Arguments)
        {
            constexpr bool     ShouldDoubleQuoteString = true;
            const std::string& ArgumentString          = Argument.ToString(ShouldDoubleQuoteString);
            ImGui::SameLine();
            ImGui::Text(ArgumentString.c_str());
        }
    }
}

std::vector<std::shared_ptr<const HTNMethodNode>>::const_iterator FindTopLevelMethodNodeByID(
    const std::string& inID, const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes)
{
    return std::find_if(inMethodNodes.begin(), inMethodNodes.end(), [&](const std::shared_ptr<const HTNMethodNode>& inMethodNode) {
        if (!inMethodNode)
        {
            return false;
        }

        if (!inMethodNode->IsTopLevel())
        {
            return false;
        }

        return (inID == inMethodNode->GetID());
    });
}

void RenderDecompositionByPlanningQuery(HTNPlanningQuery& inPlanningQuery, const std::vector<std::shared_ptr<const HTNMethodNode>>* inMethodNodes,
                                        HTNDecompositionNode& ioSelectedNode, bool& ioShouldPrintFullTooltip)
{
    if (inMethodNodes)
    {
        const auto TopLevelMethodNode = FindTopLevelMethodNodeByID(inPlanningQuery.mEntryPointID, *inMethodNodes);
        if (TopLevelMethodNode == inMethodNodes->end())
        {
            inPlanningQuery.mEntryPointID.clear();
        }
    }
    else
    {
        inPlanningQuery.mEntryPointID.clear();
    }

    if (ImGui::BeginCombo("Entry Point", inPlanningQuery.mEntryPointID.c_str(), HTNImGuiHelpers::kDefaultComboFlags))
    {
        if (inMethodNodes)
        {
            for (const std::shared_ptr<const HTNMethodNode>& MethodNode : *inMethodNodes)
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
                const bool        IsSelected   = (inPlanningQuery.mEntryPointID == MethodNodeID);
                if (ImGui::Selectable(MethodNodeID.c_str(), IsSelected, HTNImGuiHelpers::kDefaultSelectableFlags))
                {
                    inPlanningQuery.mEntryPointID = MethodNodeID;
                }

                if (IsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::Button("Decompose"))
    {
        inPlanningQuery.mLastDecompositionResult =
            static_cast<HTNOperationResult>(inPlanningQuery.mPlanningUnit->ExecuteTopLevelMethod(inPlanningQuery.mEntryPointID));

        ioSelectedNode = HTNDecompositionNode();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry point of the parsed domain using the parsed world state");
    }

    RenderOperationResult(inPlanningQuery.mLastDecompositionResult);

    ImGui::Separator();

    ImGui::Checkbox("Full Tooltip", &ioShouldPrintFullTooltip);
    if (HTNImGuiHelpers::IsCurrentItemHovered())
    {
        ImGui::SetTooltip("Whether to display all variables or only the parameters or arguments in the tooltip of the hovered line");
    }

    if (HTNOperationResult::SUCCEEDED != inPlanningQuery.mLastDecompositionResult)
    {
        return;
    }

    const ImVec2 DecompositionChildSize = ImVec2(500.f, 350.f);
    ImGui::BeginChild("DecompositionChild", DecompositionChildSize, false, ImGuiWindowFlags_HorizontalScrollbar);

    const std::shared_ptr<const HTNDomainNode>& LastDomainNode   = inPlanningQuery.mPlanningUnit->GetLastDomainNode();
    const std::string&                          LastEntryPointID = inPlanningQuery.mPlanningUnit->GetLastEntryPointID();
    const HTNDecompositionSnapshotDebug&        LastDecompositionSnapshot =
        inPlanningQuery.mPlanningUnit->GetLastDecompositionContext().GetDecompositionSnapshot();
    HTNDecompositionPrinter DecompositionPrinter =
        HTNDecompositionPrinter(LastDomainNode, LastEntryPointID, LastDecompositionSnapshot, ioShouldPrintFullTooltip);
    DecompositionPrinter.Print(ioSelectedNode);

    ImGui::EndChild();

    // Watch window
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    const ImVec2 WatchWindowChildSize = ImVec2(500.f, 150.f);
    ImGui::BeginChild("WatchWindowChild", WatchWindowChildSize, true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Watch Window", false))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    HTNDecompositionWatchWindowPrinter DecompositionWatchWindowPrinter = HTNDecompositionWatchWindowPrinter(LastDomainNode, ioSelectedNode);
    DecompositionWatchWindowPrinter.Print();

    ImGui::EndChild();
    ImGui::PopStyleVar();
}
} // namespace

void HTNDebuggerWindow::Render(bool& _IsOpen)
{
    if (ImGui::Begin("HTN Debugger Window", &_IsOpen, HTNImGuiHelpers::kDefaultWindowFlags))
    {
        if (ImGui::BeginTabBar("Tab Bar", HTNImGuiHelpers::kDefaultTabBarFlags))
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
    if (ImGui::BeginTabBar("ActivePlan", HTNImGuiHelpers::kDefaultTabBarFlags))
    {
        if (ImGui::BeginTabItem("Main"))
        {
            RenderActivePlanByPlanningUnit(*mMainPlanningUnit);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Upper Body"))
        {
            RenderActivePlanByPlanningUnit(*mUpperBodyPlanningUnit);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
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

    static HTNPlanningQuery sMainPlanningQuery = HTNPlanningQuery(mMainPlanningUnit);
    if (sMainPlanningQuery.mEntryPointID.empty())
    {
        if (MethodNodes)
        {
            static std::string DefaultMainTopLevelMethodID = "behave";
            const auto         TopLevelMethodNode          = FindTopLevelMethodNodeByID(DefaultMainTopLevelMethodID, *MethodNodes);
            if (TopLevelMethodNode != MethodNodes->end())
            {
                sMainPlanningQuery.mEntryPointID = (*TopLevelMethodNode)->GetID();
            }
        }
    }

    static HTNPlanningQuery sUpperBodyPlanningQuery = HTNPlanningQuery(mUpperBodyPlanningUnit);
    if (sUpperBodyPlanningQuery.mEntryPointID.empty())
    {
        if (MethodNodes)
        {
            static std::string DefaultUpperBodyTopLevelMethodID = "behave_upper_body";
            const auto         TopLevelMethodNode               = FindTopLevelMethodNodeByID(DefaultUpperBodyTopLevelMethodID, *MethodNodes);
            if (TopLevelMethodNode != MethodNodes->end())
            {
                sUpperBodyPlanningQuery.mEntryPointID = (*TopLevelMethodNode)->GetID();
            }
        }
    }

    static std::vector<HTNPlanningQuery*> sPlanningQueries = {&sMainPlanningQuery, &sUpperBodyPlanningQuery};

    static HTNDecompositionNode MainSelectedNode;
    static HTNDecompositionNode UpperBodySelectedNode;

    if (ImGui::Button("Decompose"))
    {
        std::for_each(std::execution::par, sPlanningQueries.begin(), sPlanningQueries.end(), [this](HTNPlanningQuery* inPlanningQuery) {
            inPlanningQuery->mLastDecompositionResult =
                static_cast<HTNOperationResult>(inPlanningQuery->mPlanningUnit->ExecuteTopLevelMethod(inPlanningQuery->mEntryPointID));
        });

        MainSelectedNode      = HTNDecompositionNode();
        UpperBodySelectedNode = HTNDecompositionNode();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry points of the parsed domain using the parsed world state");
    }

    if (ImGui::BeginTabBar("Decomposition", HTNImGuiHelpers::kDefaultTabBarFlags))
    {
        static bool ShouldPrintFullTooltip = false;

        if (ImGui::BeginTabItem("Main"))
        {
            RenderDecompositionByPlanningQuery(sMainPlanningQuery, MethodNodes, MainSelectedNode, ShouldPrintFullTooltip);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Upper Body"))
        {
            RenderDecompositionByPlanningQuery(sUpperBodyPlanningQuery, MethodNodes, UpperBodySelectedNode, ShouldPrintFullTooltip);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void HTNDebuggerWindow::RenderDomain()
{
    static const std::string     DomainsDirectoryName = "Domains";
    static const std::string     DomainFileExtension  = ".domain";
    static std::filesystem::path SelectedDomainFilePath;
    RenderFileSelector(DomainsDirectoryName, DomainFileExtension, SelectedDomainFilePath);

    static HTNOperationResult LastParseDomainResult = HTNOperationResult::NONE;
    if (ImGui::Button("Parse"))
    {
        LastParseDomainResult = static_cast<HTNOperationResult>(mPlannerHook->ParseDomainFile(SelectedDomainFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected domain file");
    }

    RenderOperationResult(LastParseDomainResult);

    ImGui::Separator();

    if (HTNOperationResult::SUCCEEDED != LastParseDomainResult)
    {
        return;
    }

    static HTNDomainPrinter                     DomainPrinter;
    const std::shared_ptr<const HTNDomainNode>& DomainNode = mPlannerHook->GetDomainNode();
    DomainPrinter.Print(DomainNode);
}

void HTNDebuggerWindow::RenderWorldState()
{
    static const std::string     WorldStatesDirectoryName = "WorldStates";
    static const std::string     WorldStateFileExtension  = ".worldstate";
    static std::filesystem::path SelectedWorldStateFilePath;
    RenderFileSelector(WorldStatesDirectoryName, WorldStateFileExtension, SelectedWorldStateFilePath);

    static HTNOperationResult LastImportWorldStateResult = HTNOperationResult::NONE;
    if (ImGui::Button("Parse"))
    {
        LastImportWorldStateResult = static_cast<HTNOperationResult>(mDatabaseHook->ParseWorldStateFile(SelectedWorldStateFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected world state file");
    }

    RenderOperationResult(LastImportWorldStateResult);

    ImGui::Separator();

    static ImGuiTextFilter TextFilter;
    TextFilter.Draw("##");

    if (HTNOperationResult::SUCCEEDED != LastImportWorldStateResult)
    {
        return;
    }

    static const HTNWorldStatePrinter WorldStatePrinter;
    const HTNWorldState&              WorldState = mDatabaseHook->GetWorldState();
    WorldStatePrinter.Print(WorldState, TextFilter);
}
#endif
