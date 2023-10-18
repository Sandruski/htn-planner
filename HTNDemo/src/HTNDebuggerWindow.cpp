#include "HTNDebuggerWindow.h"

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeParameterArgumentIDsPrinter.h"
#include "Domain/HTNDecompositionNodeParameterArgumentValuesPrinter.h"
#include "Domain/HTNDecompositionPrinter.h"
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
#include "imgui_stdlib.h"

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
        ImGui::SameLine();
        ImGui::TextColored(HTNImGuiHelpers::kFailColor, "FAILED");
    }
    else if (inOperationResult == HTNOperationResult::SUCCEEDED)
    {
        ImGui::SameLine();
        ImGui::TextColored(HTNImGuiHelpers::kSuccessColor, "SUCCEEDED");
    }
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

    ImGui::Text("Entry Points");

    struct HTNPlanningQuery
    {
        explicit HTNPlanningQuery(HTNPlanningUnit* inPlanningUnit) : mPlanningUnit(inPlanningUnit)
        {
        }

        HTNPlanningUnit*   mPlanningUnit = nullptr;
        std::string        mEntryPointID;
        HTNOperationResult mLastDecompositionResult = HTNOperationResult::NONE;
    };

    static HTNPlanningQuery sMainPlanningQuery = HTNPlanningQuery(mMainPlanningUnit);

    if (sMainPlanningQuery.mEntryPointID.empty())
    {
        if (MethodNodes)
        {
            if (!MethodNodes->empty())
            {
                sMainPlanningQuery.mEntryPointID = (*MethodNodes)[0]->GetID();
            }
        }
    };

    static HTNPlanningQuery              sUpperBodyPlanningQuery = HTNPlanningQuery(mUpperBodyPlanningUnit);
    static std::vector<HTNPlanningQuery> sPlanningQueries        = {sMainPlanningQuery, sUpperBodyPlanningQuery};

    for (HTNPlanningQuery& PlanningQuery : sPlanningQueries)
    {
        if (MethodNodes)
        {
            const auto MethodNode =
                std::find_if(MethodNodes->begin(), MethodNodes->end(), [&](const std::shared_ptr<const HTNMethodNode>& inMethodNode) {
                    if (!inMethodNode)
                    {
                        return false;
                    }

                    if (!inMethodNode->IsTopLevel())
                    {
                        return false;
                    }

                    return (PlanningQuery.mEntryPointID == inMethodNode->GetID());
                });

            if (MethodNode == MethodNodes->end())
            {
                PlanningQuery.mEntryPointID.clear();
            }
        }
        else
        {
            PlanningQuery.mEntryPointID.clear();
        }

        const std::string& PlanningUnitID = PlanningQuery.mPlanningUnit->GetID();
        if (ImGui::BeginCombo(PlanningUnitID.c_str(), PlanningQuery.mEntryPointID.c_str(), HTNImGuiHelpers::kDefaultComboFlags))
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
                    const bool        IsSelected   = (PlanningQuery.mEntryPointID == MethodNodeID);
                    if (ImGui::Selectable(MethodNodeID.c_str(), IsSelected, HTNImGuiHelpers::kDefaultSelectableFlags))
                    {
                        PlanningQuery.mEntryPointID = MethodNodeID;
                    }

                    if (IsSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }

            ImGui::EndCombo();
        }

        RenderOperationResult(PlanningQuery.mLastDecompositionResult);
    }

    if (ImGui::Button("Decompose"))
    {
        std::for_each(std::execution::par, sPlanningQueries.begin(), sPlanningQueries.end(), [this](HTNPlanningQuery& inPlanningQuery) {
            inPlanningQuery.mLastDecompositionResult =
                static_cast<HTNOperationResult>(inPlanningQuery.mPlanningUnit->ExecuteTopLevelMethod(inPlanningQuery.mEntryPointID));
        });
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("Decompose the selected entry points of the parsed domain using the parsed world state");
    }

    ImGui::Separator();

    for (const HTNPlanningQuery& PlanningQuery : sPlanningQueries)
    {
        if (HTNOperationResult::SUCCEEDED != PlanningQuery.mLastDecompositionResult)
        {
            continue;
        }

        const std::string PlanningUnitID = PlanningQuery.mPlanningUnit->GetID();
        ImGui::Text(PlanningUnitID.c_str());
        ImGui::Spacing();

        const ImVec2 ContentRegionAvail = ImGui::GetContentRegionAvail();

        // Decomposition window
        const ImVec2 DecompositionChildSize = ImVec2(ContentRegionAvail.x, 350.f);
        ImGui::BeginChild("DecompositionChild", DecompositionChildSize, false, ImGuiWindowFlags_HorizontalScrollbar);

        const std::shared_ptr<const HTNDomainNode>& LastDomainNode   = PlanningQuery.mPlanningUnit->GetLastDomainNode();
        const std::string&                          LastEntryPointID = PlanningQuery.mPlanningUnit->GetLastEntryPointID();
        const HTNDecompositionSnapshotDebug&        LastDecompositionSnapshot =
            PlanningQuery.mPlanningUnit->GetLastDecompositionContext().GetDecompositionSnapshot();
        HTNDecompositionPrinter     DecompositionPrinter = HTNDecompositionPrinter(LastDomainNode, LastEntryPointID, LastDecompositionSnapshot);
        static HTNDecompositionNode SelectedNode;
        DecompositionPrinter.Print(SelectedNode);

        ImGui::EndChild();

        // Watch window
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        const ImVec2 WatchChildSize = ImVec2(ContentRegionAvail.x, 150.f);
        ImGui::BeginChild("WatchChild", WatchChildSize, true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Watch Window", false))
            {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (ImGui::BeginTable("WatchWindow", 2, HTNImGuiHelpers::kDefaultTableFlags))
        {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            HTNDecompositionNodeParameterArgumentIDsPrinter NodeParameterArgumentIDsPrinter =
                HTNDecompositionNodeParameterArgumentIDsPrinter(SelectedNode);
            NodeParameterArgumentIDsPrinter.Print();

            ImGui::TableNextColumn();
            HTNDecompositionNodeParameterArgumentValuesPrinter NodeParameterArgumentValuesPrinter =
                HTNDecompositionNodeParameterArgumentValuesPrinter(LastDomainNode, SelectedNode);
            NodeParameterArgumentValuesPrinter.Print();

            ImGui::EndTable();
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
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

    static const HTNWorldStatePrinter WorldStatePrinter;
    const HTNWorldState&              WorldState = mDatabaseHook->GetWorldState();
    WorldStatePrinter.Print(WorldState, TextFilter);
}
#endif
