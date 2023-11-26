// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "HTNDebuggerWindow.h"

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Core/HTNFileHelpers.h"
#include "Core/HTNPathHelpers.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "Domain/Interpreter/HTNDomainInterpreter.h"
#include "Domain/Interpreter/HTNTaskResult.h"
#include "Domain/Nodes/HTNDomainNode.h"
#include "Domain/Nodes/HTNMethodNode.h"
#include "Domain/Printer/HTNDecompositionPrinterContext.h"
#include "Domain/Printer/HTNDecompositionWatchWindowPrinterContext.h"
#include "Domain/Printer/HTNDomainPrinter.h"
#include "Domain/Printer/HTNDomainPrinterContext.h"
#include "Domain/Printer/HTNNodeInstance.h"
#include "HTNImGuiHelpers.h"
#include "Hook/HTNDatabaseHook.h"
#include "Hook/HTNPlannerHook.h"
#include "Hook/HTNPlanningUnit.h"
#include "Window/HTNOperationResultHelpers.h"
#include "WorldState/Printer/HTNWorldStatePrinterContext.h"

#include "imgui.h"

#include <execution>

namespace
{
void RefreshSelectedFilePath(const std::vector<std::filesystem::path>& inFilePaths, std::filesystem::path& ioSelectedFilePath)
{
    if (!ioSelectedFilePath.empty())
    {
        const auto It = std::find_if(inFilePaths.begin(), inFilePaths.end(),
                                     [&](const std::filesystem::path& inFilePath) { return ioSelectedFilePath == inFilePath; });

        if (It == inFilePaths.end())
        {
            ioSelectedFilePath.clear();
        }
    }

    if (ioSelectedFilePath.empty())
    {
        if (!inFilePaths.empty())
        {
            ioSelectedFilePath = inFilePaths[0];
        }
    }
}

void RenderFileSelector(const std::string& inDirectoryName, const std::string& inFileExtension, std::filesystem::path& ioSelectedFilePath)
{
    const std::filesystem::path        DirectoryPath = HTNFileHelpers::MakeAbsolutePath(inDirectoryName);
    std::vector<std::filesystem::path> FilePaths;
    HTNFileHelpers::CollectFilePaths(DirectoryPath, inFileExtension, FilePaths);

    RefreshSelectedFilePath(FilePaths, ioSelectedFilePath);

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

void RefreshSelectedEntryPointID(const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, HTNPlanningQuery& ioPlanningQuery)
{
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
        const HTNPlanningUnit& PlanningUnit            = ioPlanningQuery.GetPlanningUnit();
        const std::string&     DefaultTopLevelMethodID = PlanningUnit.GetDefaultTopLevelMethodID();
        const auto             DefaultTopLevelMethodNodeIt =
            std::find_if(inMethodNodes.begin(), inMethodNodes.end(), [&](const std::shared_ptr<const HTNMethodNode>& inMethodNode) {
                if (!inMethodNode->IsTopLevel())
                {
                    return false;
                }

                const std::string MethodID = inMethodNode->GetID();
                return DefaultTopLevelMethodID == MethodID;
            });

        if (DefaultTopLevelMethodNodeIt != inMethodNodes.end())
        {
            ioPlanningQuery.SetEntryPointID(DefaultTopLevelMethodID);
            return;
        }

        const auto FirstTopLevelMethodNodeIt =
            std::find_if(inMethodNodes.begin(), inMethodNodes.end(),
                         [&](const std::shared_ptr<const HTNMethodNode>& inMethodNode) { return inMethodNode->IsTopLevel(); });

        if (FirstTopLevelMethodNodeIt != inMethodNodes.end())
        {
            const std::shared_ptr<const HTNMethodNode>& FirstTopLevelMethodNode = *FirstTopLevelMethodNodeIt;
            const std::string                           FirstTopLevelMethodID   = FirstTopLevelMethodNode->GetID();
            ioPlanningQuery.SetEntryPointID(FirstTopLevelMethodID);
            return;
        }
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

    const bool IsLastDecompositionOperationSuccessful = inPlanningQuery.IsLastDecompositionOperationSuccessful();
    if (IsLastDecompositionOperationSuccessful)
    {
        const HTNPlanningUnit&        PlanningUnit         = inPlanningQuery.GetPlanningUnit();
        const HTNDecompositionRecord& CurrentDecomposition = PlanningUnit.GetLastDecomposition();
        const HTNPlan&                Plan                 = CurrentDecomposition.GetPlan();

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
                                     HTNPlanningUnit& ioSecondaryPlanningUnit)
    : mDatabaseHook(ioDatabaseHook), mPlannerHook(ioPlannerHook), mMainPlanningUnit(ioMainPlanningUnit),
      mSecondaryPlanningUnit(ioSecondaryPlanningUnit)
{
}

void HTNDebuggerWindow::Init()
{
    // Parse world state
    const std::filesystem::path        WorldStatesDirectoryPath = HTNFileHelpers::MakeAbsolutePath(HTNFileHelpers::kWorldStatesDirectoryName);
    std::vector<std::filesystem::path> WorldStateFilePaths;
    HTNFileHelpers::CollectFilePaths(WorldStatesDirectoryPath, HTNFileHelpers::kWorldStateFileExtension, WorldStateFilePaths);
    RefreshSelectedFilePath(WorldStateFilePaths, mSelectedWorldStateFilePath);

    mLastParseWorldStateFileOperationResult =
        static_cast<const HTNOperationResult>(mDatabaseHook.ParseWorldStateFile(mSelectedWorldStateFilePath.string()));

    // Parse domain
    const std::filesystem::path        DomainsDirectoryPath = HTNFileHelpers::MakeAbsolutePath(HTNFileHelpers::kDomainsDirectoryName);
    std::vector<std::filesystem::path> DomainFilePaths;
    HTNFileHelpers::CollectFilePaths(DomainsDirectoryPath, HTNFileHelpers::kDomainFileExtension, DomainFilePaths);
    RefreshSelectedFilePath(DomainFilePaths, mSelectedDomainFilePath);

    mLastParseDomainFileOperationResult = static_cast<const HTNOperationResult>(mPlannerHook.ParseDomainFile(mSelectedDomainFilePath.string()));

    // Decompose
    std::vector<std::shared_ptr<const HTNMethodNode>> MethodNodes;
    const std::shared_ptr<const HTNDomainNode>&       DomainNode = mPlannerHook.GetDomainNode();
    if (DomainNode)
    {
        if (DomainNode->IsTopLevel())
        {
            MethodNodes = DomainNode->GetMethodNodes();
        }
    }

    // Main planning unit
    RefreshSelectedEntryPointID(MethodNodes, mMainPlanningQuery);
    DecomposePlanningQuery(mMainPlanningQuery);

    // Secondary planning unit
    RefreshSelectedEntryPointID(MethodNodes, mSecondaryPlanningQuery);
    DecomposePlanningQuery(mSecondaryPlanningQuery);
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
        if (ImGui::BeginTabItem("Main Planning Unit"))
        {
            RenderActivePlanByPlanningQuery(mMainPlanningQuery);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Secondary Planning Unit"))
        {
            RenderActivePlanByPlanningQuery(mSecondaryPlanningQuery);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void HTNDebuggerWindow::RenderDecomposition()
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

    RefreshSelectedEntryPointID(MethodNodes, mMainPlanningQuery);
    RefreshSelectedEntryPointID(MethodNodes, mSecondaryPlanningQuery);

    if (ImGui::Button("Decompose All"))
    {
        std::for_each(std::execution::par, mPlanningQueries.begin(), mPlanningQueries.end(), [this](HTNPlanningQuery* ioPlanningQuery) {
            const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
            DecomposePlanningQuery(*ioPlanningQuery);
        });

        ResetDecompositionState(mMainNodeStates, mMainChoicePointNodeStates, mMainSelectedNodeInstance);
        ResetDecompositionState(mSecondaryNodeStates, mSecondaryChoicePointNodeStates, mSecondarySelectedNodeInstance);
    }

    static constexpr ImGuiHoveredFlags ButtonHoveredFlags = ImGuiHoveredFlags_ForTooltip;
    if (ImGui::IsItemHovered(ButtonHoveredFlags))
    {
        ImGui::SetTooltip("Decompose all selected entry points of the parsed domain using the parsed world state");
    }

    if (ImGui::BeginTabBar("Decomposition"))
    {
        if (ImGui::BeginTabItem("Main Planning Unit"))
        {
            {
                const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
                RenderDecompositionByPlanningQuery(MethodNodes, mMainPlanningQuery, mMainNodeStates, mMainChoicePointNodeStates,
                                                   mMainSelectedNodeInstance);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Secondary Planning Unit"))
        {
            {
                const std::lock_guard<std::mutex> Lock(mPlanningQueryMutex);
                RenderDecompositionByPlanningQuery(MethodNodes, mSecondaryPlanningQuery, mSecondaryNodeStates, mSecondaryChoicePointNodeStates,
                                                   mSecondarySelectedNodeInstance);
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
    TextFilter.Draw("Search");

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
                                                           HTNPlanningQuery& ioPlanningQuery, HTNNodeStates& ioNodeStates,
                                                           HTNChoicePointNodeStates& ioChoicePointNodeStates, HTNNodeInstance& ioSelectedNodeInstance)
{
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
        DecomposePlanningQuery(ioPlanningQuery);

        ResetDecompositionState(ioNodeStates, ioChoicePointNodeStates, ioSelectedNodeInstance);
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

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            static constexpr ImGuiHoveredFlags MenuItemHoveredFlags = ImGuiHoveredFlags_ForTooltip;

            if (ImGui::MenuItem("Reset", "R"))
            {
                ResetDecompositionState(ioNodeStates, ioChoicePointNodeStates, ioSelectedNodeInstance);
            }

            if (ImGui::IsItemHovered(MenuItemHoveredFlags))
            {
                ImGui::SetTooltip("Display the successful decomposition");
            }

            if (ImGui::MenuItem("Unselect", "U"))
            {
                ResetSelectedNodeInstance(ioSelectedNodeInstance);
            }

            if (ImGui::IsItemHovered(MenuItemHoveredFlags))
            {
                ImGui::SetTooltip("Unselect the selected line");
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

    static constexpr bool ShouldRepeat = false;
    if (ImGui::IsKeyPressed(ImGuiKey_R, ShouldRepeat))
    {
        ResetDecompositionState(ioNodeStates, ioChoicePointNodeStates, ioSelectedNodeInstance);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_U, ShouldRepeat))
    {
        ResetSelectedNodeInstance(ioSelectedNodeInstance);
    }

    const std::shared_ptr<const HTNDomainNode>& LastDomainNode                         = ioPlanningQuery.GetLastDomainNode();
    const std::string&                          LastEntryPointID                       = ioPlanningQuery.GetLastEntryPointID();
    const HTNPlanningUnit&                      PlanningUnit                           = ioPlanningQuery.GetPlanningUnit();
    const bool                                  IsLastDecompositionOperationSuccessful = ioPlanningQuery.IsLastDecompositionOperationSuccessful();
    const HTNDecompositionResult&               LastDecompositionResult                = PlanningUnit.GetLastDecompositionResult();
    const bool                                  ShouldIgnoreImGuiState                 = !mIsDecompositionCurrentTab;
    HTNDecompositionPrinterContext              DecompositionPrinterContext =
        HTNDecompositionPrinterContext(LastDomainNode, LastEntryPointID, LastDecompositionResult, IsLastDecompositionOperationSuccessful,
                                       mTooltipMode, ShouldIgnoreImGuiState, ioNodeStates, ioChoicePointNodeStates, ioSelectedNodeInstance);
    mDecompositionPrinter.Print(DecompositionPrinterContext);

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

    HTNDecompositionWatchWindowPrinterContext DecompositionWatchWindowPrinterContext =
        HTNDecompositionWatchWindowPrinterContext(LastDomainNode, ioSelectedNodeInstance);
    mDecompositionWatchWindowPrinter.Print(DecompositionWatchWindowPrinterContext);

    ImGui::EndChild();
}

void HTNDebuggerWindow::ResetDecompositionState(HTNNodeStates& ioNodeStates, HTNChoicePointNodeStates& ioChoicePointNodeStates,
                                                HTNNodeInstance& ioSelectedNodeInstance)
{
    ioNodeStates.clear();
    ioChoicePointNodeStates.clear();
    ioSelectedNodeInstance = HTNNodeInstance();
}

void HTNDebuggerWindow::ResetSelectedNodeInstance(HTNNodeInstance& ioSelectedNodeInstance)
{
    ioSelectedNodeInstance = HTNNodeInstance();
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
