// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionPrinter.h"
#include "Domain/Printer/HTNDecompositionTooltipMode.h"
#include "Domain/Printer/HTNDecompositionWatchWindowPrinter.h"
#include "Domain/Printer/HTNDomainPrinter.h"
#include "Domain/Printer/HTNNodeInstance.h"
#include "Domain/Printer/HTNNodeState.h"
#include "HTNCoreMinimal.h"
#include "Window/HTNOperationResult.h"
#include "Window/HTNPlanningQuery.h"
#include "WorldState/Printer/HTNWorldStatePrinter.h"

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

/*
 * Debugger window of the HTN planner of an AI
 */
class HTNDebuggerWindow
{
public:
    explicit HTNDebuggerWindow(HTNDatabaseHook& ioDatabaseHook, HTNPlannerHook& ioPlannerHook, HTNPlanningUnit& ioMainPlanningUnit,
                               HTNPlanningUnit& ioSecondaryPlanningUnit);

    void Init();
    void Render();

private:
    void RenderActivePlan();
    void RenderDecomposition();
    void RenderDomain();
    void RenderWorldState();

    void RenderDecompositionByPlanningQuery(const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, HTNPlanningQuery& ioPlanningQuery,
                                            HTNNodeStates& ioNodeStates, HTNChoicePointNodeStates& ioChoicePointNodeStates,
                                            HTNNodeInstance& ioSelectedNodeInstance);

    void ResetDecompositionState(HTNNodeStates& ioNodeStates, HTNChoicePointNodeStates& ioChoicePointNodeStates,
                                 HTNNodeInstance& ioSelectedNodeInstance);
    void ResetSelectedNodeInstance(HTNNodeInstance& ioSelectedNodeInstance);

    HTN_NODISCARD bool IsLastParseWorldStateFileOperationSuccessful() const;
    HTN_NODISCARD bool IsLastParseDomainFileOperationSuccessful() const;

    HTNDatabaseHook& mDatabaseHook;
    HTNPlannerHook&  mPlannerHook;

    // Two planning units as an example, but there could be as many as parts in the body of the AI. For instance, a robot with 8 articulated arms
    // could have 8 planning units, one for each arm
    HTNPlanningUnit& mMainPlanningUnit;
    HTNPlanningUnit& mSecondaryPlanningUnit;

    std::mutex                     mPlanningQueryMutex;
    HTNPlanningQuery               mMainPlanningQuery      = HTNPlanningQuery(mMainPlanningUnit);
    HTNPlanningQuery               mSecondaryPlanningQuery = HTNPlanningQuery(mSecondaryPlanningUnit);
    std::vector<HTNPlanningQuery*> mPlanningQueries        = {&mMainPlanningQuery, &mSecondaryPlanningQuery};

    //----------------------------------------------------------------------//
    // WorldState Printer
    //----------------------------------------------------------------------//
    std::filesystem::path mSelectedWorldStateFilePath;
    HTNOperationResult    mLastParseWorldStateFileOperationResult = HTNOperationResult::NONE;

    //----------------------------------------------------------------------//
    // Domain Printer
    //----------------------------------------------------------------------//
    std::filesystem::path mSelectedDomainFilePath;
    HTNOperationResult    mLastParseDomainFileOperationResult = HTNOperationResult::NONE;

    //----------------------------------------------------------------------//
    // Decomposition Printer
    //----------------------------------------------------------------------//
    HTNNodeStates               mMainNodeStates;
    HTNNodeStates               mSecondaryNodeStates;
    HTNChoicePointNodeStates    mMainChoicePointNodeStates;
    HTNChoicePointNodeStates    mSecondaryChoicePointNodeStates;
    HTNNodeInstance             mMainSelectedNodeInstance;
    HTNNodeInstance             mSecondarySelectedNodeInstance;
    HTNDecompositionTooltipMode mTooltipMode               = HTNDecompositionTooltipMode::REGULAR;
    bool                        mIsDecompositionCurrentTab = false;

    const HTNWorldStatePrinter               mWorldStatePrinter               = HTNWorldStatePrinter();
    const HTNDomainPrinter                   mDomainPrinter                   = HTNDomainPrinter();
    const HTNDecompositionPrinter            mDecompositionPrinter            = HTNDecompositionPrinter();
    const HTNDecompositionWatchWindowPrinter mDecompositionWatchWindowPrinter = HTNDecompositionWatchWindowPrinter();
};
#endif
