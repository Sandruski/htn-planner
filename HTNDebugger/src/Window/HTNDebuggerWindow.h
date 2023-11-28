// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

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
#include "Window/HTNPlanningRequest.h"
#include "WorldState/Printer/HTNWorldStatePrinter.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
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

    // Initializes the state of the debugger window
    void Init();

    // Renders the contents of the debugger window
    void Render();

private:
    // Renders the active plan
    void RenderActivePlan();

    // Renders the decomposition
    void RenderDecomposition();

    // Renders the domain
    void RenderDomain();

    // Renders the world state
    void RenderWorldState();

    // Renders a decomposition
    void RenderDecompositionByPlanningRequest(const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes,
                                              HTNPlanningRequest& ioPlanningRequest, HTNNodeStates& ioNodeStates,
                                              HTNChoicePointNodeStates& ioChoicePointNodeStates, HTNNodeInstance& ioSelectedNodeInstance);

    // Resets the state of the given decomposition printer
    void ResetDecompositionPrinterState(HTNNodeStates& ioNodeStates, HTNChoicePointNodeStates& ioChoicePointNodeStates,
                                        HTNNodeInstance& ioSelectedNodeInstance);

    // Resets the selected node instance in a decomposition printer
    void ResetSelectedNodeInstance(HTNNodeInstance& ioSelectedNodeInstance);

    // Returns whether the last parse world state file operation is successful
    HTN_NODISCARD bool IsLastParseWorldStateFileOperationSuccessful() const;

    // Returns whether the last parse domain file operation is successful
    HTN_NODISCARD bool IsLastParseDomainFileOperationSuccessful() const;

    HTNDatabaseHook& mDatabaseHook;
    HTNPlannerHook&  mPlannerHook;

    // Two planning units as an example, but there could be as many as parts in the body of the AI. For instance, a robot with 8 articulated arms
    // could have 8 planning units, one for each arm
    HTNPlanningUnit& mMainPlanningUnit;
    HTNPlanningUnit& mSecondaryPlanningUnit;

    std::mutex                       mPlanningRequestMutex;
    HTNPlanningRequest               mMainPlanningRequest      = HTNPlanningRequest(mMainPlanningUnit);
    HTNPlanningRequest               mSecondaryPlanningRequest = HTNPlanningRequest(mSecondaryPlanningUnit);
    std::vector<HTNPlanningRequest*> mPlanningRequests         = {&mMainPlanningRequest, &mSecondaryPlanningRequest};

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
