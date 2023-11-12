#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionNode.h"
#include "Domain/HTNDecompositionPrinter.h"
#include "Domain/HTNDecompositionWatchTooltipPrinter.h"
#include "Domain/HTNDecompositionWatchWindowPrinter.h"
#include "Domain/HTNDomainPrinter.h"
#include "HTNDebuggerWindowHelpers.h"
#include "WorldState/HTNWorldStatePrinter.h"

#include <filesystem>
#include <memory>
#include <vector>
#include <mutex>

class HTNDatabaseHook;
class HTNMethodNode;
class HTNPlannerHook;
class HTNPlanningUnit;

class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNDatabaseHook& inDatabaseHook, HTNPlannerHook& inPlannerHook, HTNPlanningUnit& inMainPlanningUnit,
                               HTNPlanningUnit& inUpperBodyPlanningUnit);

    void Render();

private:
    void RenderActivePlan();
    void RenderDecomposition();
    void RenderDomain();
    void RenderWorldState();

    void RenderDecompositionByPlanningQuery(HTNPlanningQuery& inPlanningQuery, const std::vector<std::shared_ptr<const HTNMethodNode>>* inMethodNodes,
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
    HTNOperationResult    mLastWorldStateFileParsingResult = HTNOperationResult::NONE;
    HTNOperationResult    mLastDomainFileParsingResult     = HTNOperationResult::NONE;

    HTNDecompositionNode        mMainSelectedNode;
    HTNDecompositionNode        mUpperBodySelectedNode;
    HTNDecompositionTooltipMode mTooltipMode = HTNDecompositionTooltipMode::REGULAR;
    bool                        mIsDecompositionCurrentTab = false;

    HTNWorldStatePrinter               mWorldStatePrinter;
    HTNDomainPrinter                   mDomainPrinter;
    HTNDecompositionPrinter            mDecompositionPrinter;
    HTNDecompositionWatchWindowPrinter mDecompositionWatchWindowPrinter;
};

inline bool HTNDebuggerWindow::IsLastWorldStateFileParsingSuccessful() const
{
    return HTNDebuggerWindowHelpers::IsOperationSuccessful(mLastWorldStateFileParsingResult);
}

inline bool HTNDebuggerWindow::IsLastDomainFileParsingSuccessful() const
{
    return HTNDebuggerWindowHelpers::IsOperationSuccessful(mLastDomainFileParsingResult);
}
#endif
