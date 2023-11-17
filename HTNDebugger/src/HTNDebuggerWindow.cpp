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
#include "HTNFileHelpers.h"
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
    HTNPlanningUnit&         PlanningUnit = ioPlanningQuery.GetPlanningUnitMutable();
    const std::string&       EntryPointID = ioPlanningQuery.GetEntryPointID();
    const HTNOperationResult Result       = static_cast<const HTNOperationResult>(PlanningUnit.ExecuteTopLevelMethod(EntryPointID));
    ioPlanningQuery.SetLastDecompositionResult(Result);
    const HTNPlannerHook&                       PlannerHook = PlanningUnit.GetPlannerHook();
    const std::shared_ptr<const HTNDomainNode>& DomainNode  = PlannerHook.GetDomainNode();
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

    if (ImGui::BeginCombo("File", ioSelectedFilePath.filename().stem().string().c_str()))
    {
        for (const std::filesystem::path& FilePath : FilePaths)
        {
            const bool IsFileSelected = (ioSelectedFilePath == FilePath);
            if (ImGui::Selectable(FilePath.filename().stem().string().c_str(), IsFileSelected))
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
    const ImVec2           ChildSize   = ImVec2(0.f, 0.f);
    const ImGuiChildFlags  ChildFlags  = ImGuiChildFlags_None;
    const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("Child", ChildSize, ChildFlags, WindowFlags);

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

    ImGui::EndChild();
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
    : mDatabaseHook(ioDatabaseHook), mPlannerHook(ioPlannerHook), mMainPlanningUnit(ioMainPlanningUnit),
      mUpperBodyPlanningUnit(ioUpperBodyPlanningUnit)
{
}

void HTNDebuggerWindow::Render()
{
    // Set the default position and size of the main window
    constexpr ImGuiCond MainWindowCondition = ImGuiCond_FirstUseEver;
    const float         FontSize            = ImGui::GetFontSize();
    const ImVec2        MainWindowPosition  = ImVec2(FontSize, FontSize);
    ImGui::SetNextWindowPos(MainWindowPosition, MainWindowCondition);
    const ImVec2 MainWindowSize = ImVec2(36.f * FontSize, 48.f * FontSize);
    ImGui::SetNextWindowSize(MainWindowSize, MainWindowCondition);

    constexpr ImGuiWindowFlags MainWindowFlags = ImGuiWindowFlags_NoSavedSettings;
    if (ImGui::Begin("HTN Debugger Window", nullptr, MainWindowFlags))
    {
        if (ImGui::BeginTabBar("Tab Bar"))
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
    }

    ImGui::End();
}

void HTNDebuggerWindow::RenderActivePlan()
{
    if (ImGui::BeginTabBar("ActivePlan"))
    {
        if (ImGui::BeginTabItem("Main"))
        {
            RenderActivePlanByPlanningUnit(mMainPlanningUnit);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Upper Body"))
        {
            RenderActivePlanByPlanningUnit(mUpperBodyPlanningUnit);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void HTNDebuggerWindow::RenderDecomposition()
{
    const std::shared_ptr<const HTNDomainNode>&              DomainNode  = mPlannerHook.GetDomainNode();
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
        ResetDecompositionPrinterState();

        std::for_each(std::execution::par, mPlanningQueries.begin(), mPlanningQueries.end(), [this](HTNPlanningQuery* ioPlanningQuery) {
            const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
            DecomposePlanningQuery(*ioPlanningQuery);
        });
    }

    constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Decompose all selected entry points of the parsed domain using the parsed world state");
    }

    if (ImGui::BeginTabBar("Decomposition"))
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
        mLastParseDomainFileResult = static_cast<const HTNOperationResult>(mPlannerHook.ParseDomainFile(mSelectedDomainFilePath.string()));
    }

    constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Parse the selected domain file");
    }

    RenderOperationResult(mLastParseDomainFileResult);

    ImGui::Separator();

    if (!IsLastDomainFileParsingSuccessful())
    {
        return;
    }

    const ImVec2           ChildSize   = ImVec2(0.f, 0.f);
    const ImGuiChildFlags  ChildFlags  = ImGuiChildFlags_None;
    const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("Child", ChildSize, ChildFlags, WindowFlags);

    const std::shared_ptr<const HTNDomainNode>& DomainNode           = mPlannerHook.GetDomainNode();
    HTNDomainPrinterContext                     DomainPrinterContext = HTNDomainPrinterContext(DomainNode);
    mDomainPrinter.Print(DomainPrinterContext);

    ImGui::EndChild();
}

void HTNDebuggerWindow::RenderWorldState()
{
    RenderFileSelector(HTNFileHelpers::WorldStatesDirectoryName, HTNFileHelpers::WorldStateFileExtension, mSelectedWorldStateFilePath);

    if (ImGui::Button("Parse"))
    {
        mLastParseWorldStateFileResult =
            static_cast<const HTNOperationResult>(mDatabaseHook.ParseWorldStateFile(mSelectedWorldStateFilePath.string()));
    }

    constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Parse the selected world state file");
    }

    RenderOperationResult(mLastParseWorldStateFileResult);

    ImGui::Separator();

    static ImGuiTextFilter TextFilter;
    TextFilter.Draw("##");

    ImGui::SameLine();
    HTNImGuiHelpers::HelpMarker("Filter usage:\n"
                                "  \"\"         display all lines\n"
                                "  \"xxx\"      display lines containing \"xxx\"\n"
                                "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                                "  \"-xxx\"     hide lines containing \"xxx\"");

    if (!IsLastWorldStateFileParsingSuccessful())
    {
        return;
    }

    const ImVec2           ChildSize   = ImVec2(0.f, 0.f);
    const ImGuiChildFlags  ChildFlags  = ImGuiChildFlags_None;
    const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("Child", ChildSize, ChildFlags, WindowFlags);

    const HTNWorldState&        WorldState               = mDatabaseHook.GetWorldState();
    HTNWorldStatePrinterContext WorldStatePrinterContext = HTNWorldStatePrinterContext(WorldState, TextFilter);
    mWorldStatePrinter.Print(WorldStatePrinterContext);

    ImGui::EndChild();
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

    if (ImGui::BeginCombo("Entry Point", EntryPointID.c_str()))
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
                if (ImGui::Selectable(MethodNodeID.c_str(), IsSelected))
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
        ResetDecompositionPrinterState();
        DecomposePlanningQuery(ioPlanningQuery);
    }

    constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Decompose the selected entry point of the parsed domain using the parsed world state");
    }

    RenderOperationResult(ioPlanningQuery.GetLastDecompositionResult());

    ImGui::Separator();

    const ImVec2& ContentRegionAvail = ImGui::GetContentRegionAvail();

    // Decomposition
    const ImVec2           DecompositionChildSize   = ImVec2(ContentRegionAvail.x, 0.7f * ContentRegionAvail.y);
    const ImGuiChildFlags  DecompositionChildFlags  = ImGuiChildFlags_None;
    const ImGuiWindowFlags DecompositionWindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("DecompositionChild", DecompositionChildSize, DecompositionChildFlags, DecompositionWindowFlags);

    bool ShouldResetDecompositionPrinterState = false;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Reset", "R"))
            {
                ShouldResetDecompositionPrinterState = true;
            }

            constexpr ImGuiHoveredFlags MenuItemHoveredFlags = ImGuiHoveredFlags_ForTooltip;
            if (ImGui::IsItemHovered(MenuItemHoveredFlags))
            {
                ImGui::SetTooltip("Display successful decomposition");
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tooltip"))
        {
            constexpr ImGuiHoveredFlags MenuItemHoveredFlags = ImGuiHoveredFlags_ForTooltip;

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

            if (ImGui::IsItemHovered(MenuItemHoveredFlags))
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

            if (ImGui::IsItemHovered(MenuItemHoveredFlags))
            {
                ImGui::SetTooltip("Display all variables of the hovered line");
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (ioPlanningQuery.IsLastDecompositionSuccessful())
    {
        constexpr bool ShouldRepeat          = false;
        ShouldResetDecompositionPrinterState = ShouldResetDecompositionPrinterState || ImGui::IsKeyPressed(ImGuiKey_R, ShouldRepeat);

        if (ShouldResetDecompositionPrinterState)
        {
            ResetDecompositionPrinterState();
        }

        const std::shared_ptr<const HTNDomainNode>& LastDomainNode            = ioPlanningQuery.GetLastDomainNode();
        const std::string&                          LastEntryPointID          = ioPlanningQuery.GetLastEntryPointID();
        const HTNPlanningUnit&                      PlanningUnit              = ioPlanningQuery.GetPlanningUnit();
        const HTNDecompositionSnapshotDebug&        LastDecompositionSnapshot = PlanningUnit.GetLastDecompositionSnapshot();
        const bool                                  ShouldIgnoreImGuiState    = !mIsDecompositionCurrentTab;
        HTNDecompositionPrinterContext              DecompositionPrinterContext =
            HTNDecompositionPrinterContext(LastDomainNode, LastEntryPointID, LastDecompositionSnapshot, mTooltipMode, ShouldIgnoreImGuiState,
                                           mNodeStates, mChoicePointNodeStates, ioSelectedNode);
        mDecompositionPrinter.Print(DecompositionPrinterContext);
    }

    ImGui::EndChild();

    // Watch window
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    const ImVec2           WatchWindowChildSize   = ImVec2(ContentRegionAvail.x, 0.3f * ContentRegionAvail.y);
    const ImGuiChildFlags  WatchWindowChildFlags  = ImGuiChildFlags_Border;
    const ImGuiWindowFlags WatchWindowWindowFlags = ImGuiWindowFlags_MenuBar;
    ImGui::BeginChild("WatchWindowChild", WatchWindowChildSize, WatchWindowChildFlags, WatchWindowWindowFlags);
    ImGui::PopStyleVar();

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
}

void HTNDebuggerWindow::ResetDecompositionPrinterState()
{
    mNodeStates.clear();
    mChoicePointNodeStates.clear();
    mMainSelectedNode = HTNDecompositionNode();
}
#endif
