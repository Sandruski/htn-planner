#include "HTNDebuggerWindow.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionPrinterContext.h"
#include "Domain/HTNDecompositionWatchWindowPrinterContext.h"
#include "Domain/HTNDomainPrinter.h"
#include "Domain/HTNDomainPrinterContext.h"
#include "Domain/Interpreter/HTNDecompositionHelpers.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Interpreter/HTNNodePath.h"
#include "Domain/Interpreter/HTNTaskResult.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Helpers/HTNFileHelpers.h"
#include "Helpers/HTNImGuiHelpers.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNPlanningUnit.h"
#include "WorldState/HTNWorldStatePrinterContext.h"

#include "imgui.h"

#include <execution>

namespace
{
void DecomposePlanningQuery(HTNPlanningQuery& ioPlanningQuery)
{
    HTNPlanningUnit*         PlanningUnit = ioPlanningQuery.GetPlanningUnitMutable();
    const std::string&       EntryPointID = ioPlanningQuery.GetEntryPointID();
    const HTNOperationResult Result       = static_cast<const HTNOperationResult>(PlanningUnit->ExecuteTopLevelMethod(EntryPointID));
    ioPlanningQuery.SetLastDecompositionResult(Result);
    const HTNPlannerHook*                       PlannerHook = PlanningUnit->GetPlannerHook();
    const std::shared_ptr<const HTNDomainNode>& DomainNode  = PlannerHook->GetDomainNode();
    ioPlanningQuery.SetLastDomainNode(DomainNode);
    ioPlanningQuery.SetLastEntryPointID(EntryPointID);
}

void RenderFileSelector(const std::string& inDirectoryName, const std::string& inFileExtension, std::filesystem::path& ioSelectedFilePath)
{
    std::vector<std::filesystem::path> FilePaths;

    const std::filesystem::path DirectoryPath = HTNFileHelpers::MakeAbsolutePath(inDirectoryName);
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
    const HTNDecompositionRecord&     CurrentDecomposition = inPlanningUnit.GetLastDecomposition();
    const std::vector<HTNTaskResult>& Plan                 = CurrentDecomposition.GetPlan();
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
} // namespace

HTNDebuggerWindow::HTNDebuggerWindow(HTNDatabaseHook& ioDatabaseHook, HTNPlannerHook& ioPlannerHook, HTNPlanningUnit& ioMainPlanningUnit,
                                     HTNPlanningUnit& ioUpperBodyPlanningUnit)
    : mDatabaseHook(&ioDatabaseHook), mPlannerHook(&ioPlannerHook), mMainPlanningUnit(&ioMainPlanningUnit),
      mUpperBodyPlanningUnit(&ioUpperBodyPlanningUnit)
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
                const auto TopLevelMethodNode = FindTopLevelMethodNodeByID(HTNDecompositionHelpers::kDefaultMainTopLevelMethodID, *MethodNodes);
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
                const auto TopLevelMethodNode = FindTopLevelMethodNodeByID(HTNDecompositionHelpers::kDefaultUpperBodyTopLevelMethodID, *MethodNodes);
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
        std::for_each(std::execution::par, mPlanningQueries.begin(), mPlanningQueries.end(), [this](HTNPlanningQuery* ioPlanningQuery) {
            const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
            DecomposePlanningQuery(*ioPlanningQuery);
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
                RenderDecompositionByPlanningQuery(MethodNodes, mMainPlanningQuery, mMainSelectedNode);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Upper Body"))
        {
            {
                const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
                RenderDecompositionByPlanningQuery(MethodNodes, mUpperBodyPlanningQuery, mUpperBodySelectedNode);
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void HTNDebuggerWindow::RenderDomain()
{
    RenderFileSelector(HTNFileHelpers::DomainsDirectoryName, HTNFileHelpers::DomainFileExtension, mSelectedDomainFilePath);

    if (ImGui::Button("Parse"))
    {
        mLastParseDomainFileResult = static_cast<const HTNOperationResult>(mPlannerHook->ParseDomainFile(mSelectedDomainFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected domain file");
    }

    RenderOperationResult(mLastParseDomainFileResult);

    ImGui::Separator();

    if (!IsLastDomainFileParsingSuccessful())
    {
        return;
    }

    const std::shared_ptr<const HTNDomainNode>& DomainNode           = mPlannerHook->GetDomainNode();
    HTNDomainPrinterContext                     DomainPrinterContext = HTNDomainPrinterContext(DomainNode);
    mDomainPrinter.Print(DomainPrinterContext);
}

void HTNDebuggerWindow::RenderWorldState()
{
    RenderFileSelector(HTNFileHelpers::WorldStatesDirectoryName, HTNFileHelpers::WorldStateFileExtension, mSelectedWorldStateFilePath);

    if (ImGui::Button("Parse"))
    {
        mLastParseWorldStateFileResult =
            static_cast<const HTNOperationResult>(mDatabaseHook->ParseWorldStateFile(mSelectedWorldStateFilePath.string()));
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Parse the selected world state file");
    }

    RenderOperationResult(mLastParseWorldStateFileResult);

    ImGui::Separator();

    static ImGuiTextFilter TextFilter;
    TextFilter.Draw("##");

    if (!IsLastWorldStateFileParsingSuccessful())
    {
        return;
    }

    const HTNWorldState&        WorldState               = mDatabaseHook->GetWorldState();
    HTNWorldStatePrinterContext WorldStatePrinterContext = HTNWorldStatePrinterContext(WorldState, TextFilter);
    mWorldStatePrinter.Print(WorldStatePrinterContext);
}

void HTNDebuggerWindow::RenderDecompositionByPlanningQuery(const std::vector<std::shared_ptr<const HTNMethodNode>>* inMethodNodes,
                                                           HTNPlanningQuery& ioPlanningQuery, HTNDecompositionNode& ioSelectedNode)
{
    const std::string& EntryPointID = ioPlanningQuery.GetEntryPointID();

    if (inMethodNodes)
    {
        const auto TopLevelMethodNode = FindTopLevelMethodNodeByID(EntryPointID, *inMethodNodes);
        if (TopLevelMethodNode == inMethodNodes->end())
        {
            ioPlanningQuery.ClearEntryPointID();
        }
    }
    else
    {
        ioPlanningQuery.ClearEntryPointID();
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
                    ioPlanningQuery.SetEntryPointID(MethodNodeID);
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
        DecomposePlanningQuery(ioPlanningQuery);
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry point of the parsed domain using the parsed world state");
    }

    RenderOperationResult(ioPlanningQuery.GetLastDecompositionResult());

    ImGui::Separator();

    // Decomposition window
    const ImVec2 DecompositionChildSize = ImVec2(500.f, 350.f);
    ImGui::BeginChild("DecompositionChild", DecompositionChildSize, false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

    bool ShouldReset = false;

    // TODO salvarez Make it work without delay (and from main)
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

    if (ioPlanningQuery.IsLastDecompositionSuccessful())
    {
        const std::shared_ptr<const HTNDomainNode>& LastDomainNode            = ioPlanningQuery.GetLastDomainNode();
        const std::string&                          LastEntryPointID          = ioPlanningQuery.GetLastEntryPointID();
        const HTNPlanningUnit*                      PlanningUnit              = ioPlanningQuery.GetPlanningUnit();
        const HTNDecompositionSnapshotDebug&        LastDecompositionSnapshot = PlanningUnit->GetLastDecompositionSnapshot();
        const bool                                  ShouldIgnoreNewNodeOpen   = !mIsDecompositionCurrentTab;
        HTNDecompositionPrinterContext              DecompositionPrinterContext =
            HTNDecompositionPrinterContext(LastDomainNode, LastEntryPointID, LastDecompositionSnapshot, mTooltipMode, ShouldIgnoreNewNodeOpen,
                                           mNodeStates, mChoicePointNodeStates, ioSelectedNode);
        mDecompositionPrinter.Print(DecompositionPrinterContext);
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

    if (ioPlanningQuery.IsLastDecompositionSuccessful())
    {
        const std::shared_ptr<const HTNDomainNode>& LastDomainNode = ioPlanningQuery.GetLastDomainNode();
        HTNDecompositionWatchWindowPrinterContext   DecompositionWatchWindowPrinterContext =
            HTNDecompositionWatchWindowPrinterContext(LastDomainNode, ioSelectedNode);
        mDecompositionWatchWindowPrinter.Print(DecompositionWatchWindowPrinterContext);
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
}
#endif
