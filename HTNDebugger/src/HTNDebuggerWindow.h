#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNFrameworkMinimal.h"
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionNodeState.h"
#include "Domain/HTNDecompositionPrinter.h"
#include "Domain/HTNDecompositionTooltipMode.h"
#include "Domain/HTNDecompositionWatchWindowPrinter.h"
#include "Domain/HTNDomainPrinter.h"
#include "Helpers/HTNDebuggerWindowHelpers.h"
#include "Helpers/HTNPlanningQuery.h"
#include "WorldState/HTNWorldStatePrinter.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class HTNDatabaseHook;
class HTNMethodNode;
class HTNPlannerHook;
class HTNPlanningUnit;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNDatabaseHook& ioDatabaseHook, HTNPlannerHook& ioPlannerHook, HTNPlanningUnit& ioMainPlanningUnit,
                               HTNPlanningUnit& ioUpperBodyPlanningUnit);

    void Render();

private:
    void RenderActivePlan();
    void RenderDecomposition();
    void RenderDomain();
    void RenderWorldState();

    void RenderDecompositionByPlanningQuery(const std::vector<std::shared_ptr<const HTNMethodNode>>* inMethodNodes, HTNPlanningQuery& ioPlanningQuery,
                                            HTNDecompositionNode& ioSelectedNode);

    void ResetDecompositionPrinterState();

    bool IsLastWorldStateFileParsingSuccessful() const;
    bool IsLastDomainFileParsingSuccessful() const;

    HTNDatabaseHook& mDatabaseHook;
    HTNPlannerHook&  mPlannerHook;
    HTNPlanningUnit& mMainPlanningUnit;
    HTNPlanningUnit& mUpperBodyPlanningUnit;

    std::mutex                     mPlanningQueryMutex;
    HTNPlanningQuery               mMainPlanningQuery      = HTNPlanningQuery(mMainPlanningUnit);
    HTNPlanningQuery               mUpperBodyPlanningQuery = HTNPlanningQuery(mUpperBodyPlanningUnit);
    std::vector<HTNPlanningQuery*> mPlanningQueries        = {&mMainPlanningQuery, &mUpperBodyPlanningQuery};

    //----------------------------------------------------------------------//
    // WorldState Printer
    //----------------------------------------------------------------------//
    std::filesystem::path mSelectedWorldStateFilePath;
    HTNOperationResult    mLastParseWorldStateFileResult = HTNOperationResult::NONE;

    //----------------------------------------------------------------------//
    // Domain Printer
    //----------------------------------------------------------------------//
    std::filesystem::path mSelectedDomainFilePath;
    HTNOperationResult    mLastParseDomainFileResult = HTNOperationResult::NONE;

    //----------------------------------------------------------------------//
    // Decomposition Printer
    //----------------------------------------------------------------------//
    std::unordered_map<std::string, HTNDecompositionNodeState>            mNodeStates;
    std::unordered_map<std::string, HTNDecompositionChoicePointNodeState> mChoicePointNodeStates;
    HTNDecompositionNode                                                  mMainSelectedNode;
    HTNDecompositionNode                                                  mUpperBodySelectedNode;
    HTNDecompositionTooltipMode                                           mTooltipMode               = HTNDecompositionTooltipMode::REGULAR;
    bool                                                                  mIsDecompositionCurrentTab = false;

    const HTNWorldStatePrinter               mWorldStatePrinter               = HTNWorldStatePrinter();
    const HTNDomainPrinter                   mDomainPrinter                   = HTNDomainPrinter();
    const HTNDecompositionPrinter            mDecompositionPrinter            = HTNDecompositionPrinter();
    const HTNDecompositionWatchWindowPrinter mDecompositionWatchWindowPrinter = HTNDecompositionWatchWindowPrinter();
};

inline bool HTNDebuggerWindow::IsLastWorldStateFileParsingSuccessful() const
{
    return HTNDebuggerWindowHelpers::IsOperationSuccessful(mLastParseWorldStateFileResult);
}

inline bool HTNDebuggerWindow::IsLastDomainFileParsingSuccessful() const
{
    return HTNDebuggerWindowHelpers::IsOperationSuccessful(mLastParseDomainFileResult);
}
#endif
