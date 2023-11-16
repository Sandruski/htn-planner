#pragma once

#ifdef HTN_DEBUG
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

    bool IsLastWorldStateFileParsingSuccessful() const;
    bool IsLastDomainFileParsingSuccessful() const;

    HTNDatabaseHook* mDatabaseHook          = nullptr;
    HTNPlannerHook*  mPlannerHook           = nullptr;
    HTNPlanningUnit* mMainPlanningUnit      = nullptr;
    HTNPlanningUnit* mUpperBodyPlanningUnit = nullptr;

    std::mutex                     mPlanningQueryMutex;
    HTNPlanningQuery               mMainPlanningQuery      = HTNPlanningQuery(mMainPlanningUnit);
    HTNPlanningQuery               mUpperBodyPlanningQuery = HTNPlanningQuery(mUpperBodyPlanningUnit);
    std::vector<HTNPlanningQuery*> mPlanningQueries        = {&mMainPlanningQuery, &mUpperBodyPlanningQuery};

    std::filesystem::path mSelectedWorldStateFilePath;
    std::filesystem::path mSelectedDomainFilePath;
    HTNOperationResult    mLastParseWorldStateFileResult = HTNOperationResult::NONE;
    HTNOperationResult    mLastParseDomainFileResult     = HTNOperationResult::NONE;

    HTNDecompositionNode                                                  mMainSelectedNode;
    HTNDecompositionNode                                                  mUpperBodySelectedNode;
    HTNDecompositionTooltipMode                                           mTooltipMode               = HTNDecompositionTooltipMode::REGULAR;
    bool                                                                  mIsDecompositionCurrentTab = false;
    std::unordered_map<std::string, HTNDecompositionNodeState>            mNodeStates;
    std::unordered_map<std::string, HTNDecompositionChoicePointNodeState> mChoicePointNodeStates;

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
