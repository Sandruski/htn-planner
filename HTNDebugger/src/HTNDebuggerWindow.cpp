// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "HTNDebuggerWindow.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNFileHelpers.h"
#include "Core/HTNPathHelpers.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionPrinterContext.h"
#include "Domain/HTNDecompositionWatchWindowPrinterContext.h"
#include "Domain/HTNDomainPrinter.h"
#include "Domain/HTNDomainPrinterContext.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Interpreter/HTNTaskResult.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Helpers/HTNImGuiHelpers.h"
#include "Helpers/HTNOperationResultHelpers.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNPlanningUnit.h"
#include "WorldState/HTNWorldStatePrinterContext.h"

#include "imgui.h"

#include <execution>

namespace
{
void RenderFileSelector(const std::string& inDirectoryName, const std::string& inFileExtension, std::filesystem::path& ioSelectedFilePath)
{
    const std::filesystem::path        DirectoryPath = HTNFileHelpers::MakeAbsolutePath(inDirectoryName);
    std::vector<std::filesystem::path> FilePaths;
    HTNFileHelpers::CollectFilePathsRecursively(DirectoryPath, inFileExtension, FilePaths);

    // Refresh selected file path
    if (!ioSelectedFilePath.empty())
    {
        const auto It = std::find_if(FilePaths.begin(), FilePaths.end(),
                                     [&](const std::filesystem::path& inFilePath) { return ioSelectedFilePath == inFilePath; });

        if (It == FilePaths.end())
        {
            ioSelectedFilePath.clear();
        }
    }

    if (ioSelectedFilePath.empty())
    {
        if (!FilePaths.empty())
        {
            ioSelectedFilePath = FilePaths[0];
        }
    }

    const std::string SelectedFilePathDisplayName = HTNFileHelpers::MakeFilePathDisplayName(ioSelectedFilePath, DirectoryPath);
    if (ImGui::BeginCombo("File", SelectedFilePathDisplayName.c_str()))
    {
        for (const std::filesystem::path& FilePath : FilePaths)
        {
            const std::string FilePathDisplayName = HTNFileHelpers::MakeFilePathDisplayName(FilePath, DirectoryPath);
            const bool        IsFileSelected      = ioSelectedFilePath == FilePath;
            if (ImGui::Selectable(FilePathDisplayName.c_str(), IsFileSelected))
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

void DecomposePlanningQuery(HTNPlanningQuery& ioPlanningQuery)
{
    HTNPlanningUnit&         PlanningUnit    = ioPlanningQuery.GetPlanningUnitMutable();
    const std::string&       EntryPointID    = ioPlanningQuery.GetEntryPointID();
    const HTNOperationResult OperationResult = static_cast<const HTNOperationResult>(PlanningUnit.ExecuteTopLevelMethod(EntryPointID));
    ioPlanningQuery.SetLastDecompositionOperationResult(OperationResult);
    const HTNPlannerHook&                       PlannerHook = PlanningUnit.GetPlannerHook();
    const std::shared_ptr<const HTNDomainNode>& DomainNode  = PlannerHook.GetDomainNode();
    ioPlanningQuery.SetLastDomainNode(DomainNode);
    ioPlanningQuery.SetLastEntryPointID(EntryPointID);
}

void RenderActivePlanByPlanningQuery(const HTNPlanningQuery& inPlanningQuery)
{
    const ImVec2           ChildSize   = ImVec2(0.f, 0.f);
    const ImGuiChildFlags  ChildFlags  = ImGuiChildFlags_None;
    const ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_HorizontalScrollbar;
    ImGui::BeginChild("Child", ChildSize, ChildFlags, WindowFlags);

    const bool IsLastDecompositionSuccessful = inPlanningQuery.IsLastDecompositionOperationSuccessful();
    if (IsLastDecompositionSuccessful)
    {
        const HTNPlanningUnit&            PlanningUnit         = inPlanningQuery.GetPlanningUnit();
        const HTNDecompositionRecord&     CurrentDecomposition = PlanningUnit.GetLastDecomposition();
        const std::vector<HTNTaskResult>& Plan                 = CurrentDecomposition.GetPlan();

        if (Plan.empty())
        {
            ImGui::Text("Empty plan");
        }

        for (const HTNTaskResult& TaskResult : Plan)
        {
            const std::string& TaskID = TaskResult.GetID();
            ImGui::Text(TaskID.c_str());

            const std::vector<HTNAtom>& Arguments = TaskResult.GetArguments();
            for (const HTNAtom& Argument : Arguments)
            {
                static constexpr bool ShouldDoubleQuoteString = true;
                const std::string&    ArgumentString          = Argument.ToString(ShouldDoubleQuoteString);
                ImGui::SameLine();
                ImGui::Text(ArgumentString.c_str());
            }
        }
    }

    ImGui::EndChild();
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
    static constexpr ImGuiCond MainWindowCondition = ImGuiCond_FirstUseEver;
    const float                FontSize            = ImGui::GetFontSize();
    const ImVec2               MainWindowPosition  = ImVec2(FontSize, FontSize);
    ImGui::SetNextWindowPos(MainWindowPosition, MainWindowCondition);
    const ImVec2 MainWindowSize = ImVec2(36.f * FontSize, 48.f * FontSize);
    ImGui::SetNextWindowSize(MainWindowSize, MainWindowCondition);

    static constexpr ImGuiWindowFlags MainWindowFlags = ImGuiWindowFlags_NoSavedSettings;
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
            RenderActivePlanByPlanningQuery(mMainPlanningQuery);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Upper Body"))
        {
            RenderActivePlanByPlanningQuery(mUpperBodyPlanningQuery);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void HTNDebuggerWindow::RenderDecomposition()
{
    if (ImGui::Button("Decompose All"))
    {
        ResetDecompositionPrinterState();

        std::for_each(std::execution::par, mPlanningQueries.begin(), mPlanningQueries.end(), [this](HTNPlanningQuery* ioPlanningQuery) {
            const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
            DecomposePlanningQuery(*ioPlanningQuery);
        });
    }

    static constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Decompose all selected entry points of the parsed domain using the parsed world state");
    }

    if (ImGui::BeginTabBar("Decomposition"))
    {
        std::vector<std::shared_ptr<const HTNMethodNode>> MethodNodes;
        const std::shared_ptr<const HTNDomainNode>&       DomainNode = mPlannerHook.GetDomainNode();
        if (DomainNode)
        {
            if (DomainNode->IsTopLevel())
            {
                MethodNodes = DomainNode->GetMethodNodes();
            }
        }

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
    RenderFileSelector(HTNFileHelpers::kDomainsDirectoryName, HTNFileHelpers::kDomainFileExtension, mSelectedDomainFilePath);

    if (ImGui::Button("Parse"))
    {
        mLastParseDomainFileOperationResult = static_cast<const HTNOperationResult>(mPlannerHook.ParseDomainFile(mSelectedDomainFilePath.string()));
    }

    static constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Parse the selected domain file");
    }

    RenderOperationResult(mLastParseDomainFileOperationResult);

    ImGui::Separator();

    if (!IsLastDomainFileParsingOperationSuccessful())
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
    RenderFileSelector(HTNFileHelpers::kWorldStatesDirectoryName, HTNFileHelpers::kWorldStateFileExtension, mSelectedWorldStateFilePath);

    if (ImGui::Button("Parse"))
    {
        mLastParseWorldStateFileOperationResult =
            static_cast<const HTNOperationResult>(mDatabaseHook.ParseWorldStateFile(mSelectedWorldStateFilePath.string()));
    }

    static constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Parse the selected world state file");
    }

    RenderOperationResult(mLastParseWorldStateFileOperationResult);

    ImGui::Separator();

    static ImGuiTextFilter TextFilter;
    TextFilter.Draw("##");

    ImGui::SameLine();
    HTNImGuiHelpers::HelpMarker("Filter usage:\n"
                                "  \"\"         display all lines\n"
                                "  \"xxx\"      display lines containing \"xxx\"\n"
                                "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                                "  \"-xxx\"     hide lines containing \"xxx\"");

    if (!IsLastWorldStateFileParsingOperationSuccessful())
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

void HTNDebuggerWindow::RenderDecompositionByPlanningQuery(const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes,
                                                           HTNPlanningQuery& ioPlanningQuery, HTNDecompositionNode& ioSelectedNode)
{
    // Refresh selected entry point ID
    if (!ioPlanningQuery.IsEntryPointIDEmpty())
    {
        const std::string& EntryPointID = ioPlanningQuery.GetEntryPointID();
        const auto It = std::find_if(inMethodNodes.begin(), inMethodNodes.end(), [&](const std::shared_ptr<const HTNMethodNode>& inMethodNode) {
            if (!inMethodNode->IsTopLevel())
            {
                return false;
            }

            const std::string MethodID = inMethodNode->GetID();
            return EntryPointID == MethodID;
        });

        if (It == inMethodNodes.end())
        {
            ioPlanningQuery.ClearEntryPointID();
        }
    }

    if (ioPlanningQuery.IsEntryPointIDEmpty())
    {
        const auto It = std::find_if(inMethodNodes.begin(), inMethodNodes.end(),
                                     [&](const std::shared_ptr<const HTNMethodNode>& inMethodNode) { return inMethodNode->IsTopLevel(); });

        if (It != inMethodNodes.end())
        {
            const std::shared_ptr<const HTNMethodNode>& TopLevelMethodNode = *It;
            const std::string                           EntryPointID       = TopLevelMethodNode->GetID();
            ioPlanningQuery.SetEntryPointID(EntryPointID);
        }
    }

    const std::string& EntryPointID = ioPlanningQuery.GetEntryPointID();
    if (ImGui::BeginCombo("Entry Point", EntryPointID.c_str()))
    {
        for (const std::shared_ptr<const HTNMethodNode>& MethodNode : inMethodNodes)
        {
            if (!MethodNode->IsTopLevel())
            {
                continue;
            }

            const std::string MethodNodeID = MethodNode->GetID();
            const bool        IsSelected   = EntryPointID == MethodNodeID;
            if (ImGui::Selectable(MethodNodeID.c_str(), IsSelected))
            {
                ioPlanningQuery.SetEntryPointID(MethodNodeID);
            }

            if (IsSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }

    if (ImGui::Button("Decompose"))
    {
        ResetDecompositionPrinterState();
        DecomposePlanningQuery(ioPlanningQuery);
    }

    static constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Decompose the selected entry point of the parsed domain using the parsed world state");
    }

    const HTNOperationResult LastDecompositionOperationResult = ioPlanningQuery.GetLastDecompositionOperationResult();
    RenderOperationResult(LastDecompositionOperationResult);

    ImGui::Separator();

    const ImVec2      ContentRegionAvail = ImGui::GetContentRegionAvail();
    const ImGuiStyle& Style              = ImGui::GetStyle();
    const ImVec2      AvailableSize      = ImVec2(ContentRegionAvail.x, ContentRegionAvail.y - Style.ItemSpacing.y);

    // Decomposition
    static constexpr float DecompositionHeightPercentage = 0.7f;
    const ImVec2           DecompositionChildSize        = ImVec2(AvailableSize.x, DecompositionHeightPercentage * AvailableSize.y);
    const ImGuiChildFlags  DecompositionChildFlags       = ImGuiChildFlags_None;
    const ImGuiWindowFlags DecompositionWindowFlags      = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar;
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

            static constexpr ImGuiHoveredFlags MenuItemHoveredFlags = ImGuiHoveredFlags_ForTooltip;
            if (ImGui::IsItemHovered(MenuItemHoveredFlags))
            {
                ImGui::SetTooltip("Display successful decomposition");
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tooltip"))
        {
            static constexpr ImGuiHoveredFlags MenuItemHoveredFlags = ImGuiHoveredFlags_ForTooltip;

            bool IsRegularTooltipMode = HTNDecompositionTooltipMode::REGULAR == mTooltipMode;
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

            bool IsFullTooltipMode = HTNDecompositionTooltipMode::FULL == mTooltipMode;
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

    const bool IsLastDecompositionOperationSuccessful = ioPlanningQuery.IsLastDecompositionOperationSuccessful();
    if (IsLastDecompositionOperationSuccessful)
    {
        static constexpr bool ShouldRepeat   = false;
        ShouldResetDecompositionPrinterState = ShouldResetDecompositionPrinterState || ImGui::IsKeyPressed(ImGuiKey_R, ShouldRepeat);

        if (ShouldResetDecompositionPrinterState)
        {
            ResetDecompositionPrinterState();
        }

        const std::shared_ptr<const HTNDomainNode>& LastDomainNode          = ioPlanningQuery.GetLastDomainNode();
        const std::string&                          LastEntryPointID        = ioPlanningQuery.GetLastEntryPointID();
        const HTNPlanningUnit&                      PlanningUnit            = ioPlanningQuery.GetPlanningUnit();
        const HTNDecompositionResult&               LastDecompositionResult = PlanningUnit.GetLastDecompositionResult();
        const bool                                  ShouldIgnoreImGuiState  = !mIsDecompositionCurrentTab;
        HTNDecompositionPrinterContext              DecompositionPrinterContext =
            HTNDecompositionPrinterContext(LastDomainNode, LastEntryPointID, LastDecompositionResult, mTooltipMode, ShouldIgnoreImGuiState,
                                           mNodeStates, mChoicePointNodeStates, ioSelectedNode);
        mDecompositionPrinter.Print(DecompositionPrinterContext);
    }

    ImGui::EndChild();

    // Watch window
    static constexpr float WatchWindowHeightPercentage = 1.f - DecompositionHeightPercentage;
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    const ImVec2           WatchWindowChildSize   = ImVec2(AvailableSize.x, WatchWindowHeightPercentage * AvailableSize.y);
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

    if (IsLastDecompositionOperationSuccessful)
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

bool HTNDebuggerWindow::IsLastWorldStateFileParsingOperationSuccessful() const
{
    return HTNOperationResultHelpers::IsOperationSuccessful(mLastParseWorldStateFileOperationResult);
}

bool HTNDebuggerWindow::IsLastDomainFileParsingOperationSuccessful() const
{
    return HTNOperationResultHelpers::IsOperationSuccessful(mLastParseDomainFileOperationResult);
}
#endif
