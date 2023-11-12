#include "HTNDebuggerWindow.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
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

#include "imgui.h"

#include <execution>

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
        const bool   Result = static_cast<const bool>(inOperationResult);
        const ImVec4 Color  = HTNImGuiHelpers::GetResultColor(Result);
        ImGui::SameLine();
        ImGui::TextColored(Color, "FAILED");
    }
    else if (inOperationResult == HTNOperationResult::SUCCEEDED)
    {
        const bool   Result = static_cast<const bool>(inOperationResult);
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

const std::string WorldStatesDirectoryName = "WorldStates";
const std::string WorldStateFileExtension  = ".worldstate";
const std::string DomainsDirectoryName     = "Domains";
const std::string DomainFileExtension      = ".domain";

const std::string DefaultMainTopLevelMethodID      = "behave";
const std::string DefaultUpperBodyTopLevelMethodID = "behave_upper_body";
} // namespace

HTNDebuggerWindow::HTNDebuggerWindow(HTNDatabaseHook& inDatabaseHook, HTNPlannerHook& inPlannerHook, HTNPlanningUnit& inMainPlanningUnit,
                                     HTNPlanningUnit& inUpperBodyPlanningUnit)
    : mDatabaseHook(&inDatabaseHook), mPlannerHook(&inPlannerHook), mMainPlanningUnit(&inMainPlanningUnit),
      mUpperBodyPlanningUnit(&inUpperBodyPlanningUnit)
{
}

void HTNDebuggerWindow::Render()
{
    if (ImGui::Begin("HTN Debugger Window", nullptr, HTNImGuiHelpers::kDefaultWindowFlags))
    {
        if (ImGui::BeginTabBar("Tab Bar", HTNImGuiHelpers::kDefaultTabBarFlags))
        {
            if (ImGui::BeginTabItem("Active Plan"))
            {
                RenderActivePlan();
                mIsDecompositionCurrentTab = false;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Decomposition"))
            {
                RenderDecomposition();
                mIsDecompositionCurrentTab = true;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Domain"))
            {
                RenderDomain();
                mIsDecompositionCurrentTab = false;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("World State"))
            {
                RenderWorldState();
                mIsDecompositionCurrentTab = false;
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

    {
        const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
        if (mMainPlanningQuery.IsEntryPointIDEmpty())
        {
            if (MethodNodes)
            {
                const auto TopLevelMethodNode = FindTopLevelMethodNodeByID(DefaultMainTopLevelMethodID, *MethodNodes);
                if (TopLevelMethodNode != MethodNodes->end())
                {
                    const std::string EntryPointID = (*TopLevelMethodNode)->GetID();
                    mMainPlanningQuery.SetEntryPointID(EntryPointID);
                }
            }
        }
    }

    {
        const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
        if (mUpperBodyPlanningQuery.IsEntryPointIDEmpty())
        {
            if (MethodNodes)
            {
                const auto TopLevelMethodNode = FindTopLevelMethodNodeByID(DefaultUpperBodyTopLevelMethodID, *MethodNodes);
                if (TopLevelMethodNode != MethodNodes->end())
                {
                    const std::string EntryPointID = (*TopLevelMethodNode)->GetID();
                    mUpperBodyPlanningQuery.SetEntryPointID(EntryPointID);
                }
            }
        }
    }

    if (ImGui::Button("Decompose All"))
    {
        std::for_each(std::execution::par, mPlanningQueries.begin(), mPlanningQueries.end(), [this](HTNPlanningQuery* inPlanningQuery) {
            const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
            HTNPlanningUnit*                  PlanningUnit = inPlanningQuery->GetPlanningUnitMutable();
            const std::string&                EntryPointID = inPlanningQuery->GetEntryPointID();
            const HTNOperationResult          Result       = static_cast<const HTNOperationResult>(PlanningUnit->ExecuteTopLevelMethod(EntryPointID));
            inPlanningQuery->SetLastDecompositionResult(Result);
        });
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose all selected entry points of the parsed domain using the parsed world state");
    }

    if (ImGui::BeginTabBar("Decomposition", HTNImGuiHelpers::kDefaultTabBarFlags))
    {
        if (ImGui::BeginTabItem("Main"))
        {
            {
                const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
                RenderDecompositionByPlanningQuery(mMainPlanningQuery, MethodNodes, mMainSelectedNode);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Upper Body"))
        {
            {
                const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
                RenderDecompositionByPlanningQuery(mUpperBodyPlanningQuery, MethodNodes, mUpperBodySelectedNode);
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void HTNDebuggerWindow::RenderDomain()
{
    RenderFileSelector(DomainsDirectoryName, DomainFileExtension, mSelectedDomainFilePath);

    if (ImGui::Button("Parse"))
    {
        mLastDomainFileParsingResult = static_cast<const HTNOperationResult>(mPlannerHook->ParseDomainFile(mSelectedDomainFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected domain file");
    }

    RenderOperationResult(mLastDomainFileParsingResult);

    ImGui::Separator();

    if (!IsLastDomainFileParsingSuccessful())
    {
        return;
    }

    const std::shared_ptr<const HTNDomainNode>& DomainNode = mPlannerHook->GetDomainNode();
    mDomainPrinter.Print(DomainNode);
}

void HTNDebuggerWindow::RenderWorldState()
{
    RenderFileSelector(WorldStatesDirectoryName, WorldStateFileExtension, mSelectedWorldStateFilePath);

    if (ImGui::Button("Parse"))
    {
        mLastWorldStateFileParsingResult =
            static_cast<const HTNOperationResult>(mDatabaseHook->ParseWorldStateFile(mSelectedWorldStateFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected world state file");
    }

    RenderOperationResult(mLastWorldStateFileParsingResult);

    ImGui::Separator();

    static ImGuiTextFilter TextFilter;
    TextFilter.Draw("##");

    if (!IsLastWorldStateFileParsingSuccessful())
    {
        return;
    }

    const HTNWorldState& WorldState = mDatabaseHook->GetWorldState();
    mWorldStatePrinter.Print(WorldState, TextFilter);
}

void HTNDebuggerWindow::RenderDecompositionByPlanningQuery(HTNPlanningQuery&                                        inPlanningQuery,
                                                           const std::vector<std::shared_ptr<const HTNMethodNode>>* inMethodNodes,
                                                           HTNDecompositionNode&                                    ioSelectedNode)
{
    const std::string& EntryPointID = inPlanningQuery.GetEntryPointID();

    if (inMethodNodes)
    {
        const auto TopLevelMethodNode = FindTopLevelMethodNodeByID(EntryPointID, *inMethodNodes);
        if (TopLevelMethodNode == inMethodNodes->end())
        {
            inPlanningQuery.ClearEntryPointID();
        }
    }
    else
    {
        inPlanningQuery.ClearEntryPointID();
    }

    if (ImGui::BeginCombo("Entry Point", EntryPointID.c_str(), HTNImGuiHelpers::kDefaultComboFlags))
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
                const bool        IsSelected   = (EntryPointID == MethodNodeID);
                if (ImGui::Selectable(MethodNodeID.c_str(), IsSelected, HTNImGuiHelpers::kDefaultSelectableFlags))
                {
                    inPlanningQuery.SetEntryPointID(MethodNodeID);
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
        HTNPlanningUnit*         PlanningUnit = inPlanningQuery.GetPlanningUnitMutable();
        const HTNOperationResult Result       = static_cast<const HTNOperationResult>(PlanningUnit->ExecuteTopLevelMethod(EntryPointID));
        inPlanningQuery.SetLastDecompositionResult(Result);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry point of the parsed domain using the parsed world state");
    }

    RenderOperationResult(inPlanningQuery.GetLastDecompositionResult());

    ImGui::Separator();

    // Decomposition window
    const ImVec2 DecompositionChildSize = ImVec2(500.f, 350.f);
    ImGui::BeginChild("DecompositionChild", DecompositionChildSize, false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

    bool ShouldReset = false;

    // TODO salvarez Make it work without delay
    /*
    if (ImGui::IsKeyDown(ImGuiKey_R))
    {
        ShouldReset = true;
    }
    */

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Reset", "R"))
            {
                ShouldReset = true;
            }

            if (HTNImGuiHelpers::IsCurrentItemHovered())
            {
                ImGui::SetTooltip("Display successful decomposition");
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tooltip"))
        {
            bool IsRegularTooltipMode = (HTNDecompositionTooltipMode::REGULAR == mTooltipMode);
            if (ImGui::MenuItem("Regular", nullptr, &IsRegularTooltipMode))
            {
                if (IsRegularTooltipMode)
                {
                    mTooltipMode = HTNDecompositionTooltipMode::REGULAR;
                }
                else
                {
                    mTooltipMode = HTNDecompositionTooltipMode::NONE;
                }
            }

            if (HTNImGuiHelpers::IsCurrentItemHovered())
            {
                ImGui::SetTooltip("Display only the parameters or arguments of the hovered line");
            }

            bool IsFullTooltipMode = (HTNDecompositionTooltipMode::FULL == mTooltipMode);
            if (ImGui::MenuItem("Full", nullptr, &IsFullTooltipMode))
            {
                if (IsFullTooltipMode)
                {
                    mTooltipMode = HTNDecompositionTooltipMode::FULL;
                }
                else
                {
                    mTooltipMode = HTNDecompositionTooltipMode::NONE;
                }
            }

            if (HTNImGuiHelpers::IsCurrentItemHovered())
            {
                ImGui::SetTooltip("Display all variables of the hovered line");
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (inPlanningQuery.IsLastDecompositionSuccessful())
    {
        const HTNPlanningUnit*                      PlanningUnit       = inPlanningQuery.GetPlanningUnit();
        const std::shared_ptr<const HTNDomainNode>& LastDomainNode     = PlanningUnit->GetLastDomainNode();
        const std::string&                          LastEntryPointID   = PlanningUnit->GetLastEntryPointID();
        const HTNDecompositionSnapshotDebug& LastDecompositionSnapshot = PlanningUnit->GetLastDecompositionContext().GetDecompositionSnapshot();
        const bool                           ShouldIgnoreNewNodeOpen   = !mIsDecompositionCurrentTab;
        mDecompositionPrinter.Print(LastDomainNode, LastEntryPointID, LastDecompositionSnapshot, mTooltipMode, ShouldIgnoreNewNodeOpen, ShouldReset,
                                    ioSelectedNode);
    }

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

    if (inPlanningQuery.IsLastDecompositionSuccessful())
    {
        const HTNPlanningUnit*                      PlanningUnit   = inPlanningQuery.GetPlanningUnit();
        const std::shared_ptr<const HTNDomainNode>& LastDomainNode = PlanningUnit->GetLastDomainNode();
        mDecompositionWatchWindowPrinter.Print(LastDomainNode, ioSelectedNode);
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
}
#endif
