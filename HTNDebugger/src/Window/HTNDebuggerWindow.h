// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Printer/HTNDecompositionNode.h"
#include "Domain/Printer/HTNDecompositionNodeState.h"
#include "Domain/Printer/HTNDecompositionPrinter.h"
#include "Domain/Printer/HTNDecompositionTooltipMode.h"
#include "Domain/Printer/HTNDecompositionWatchWindowPrinter.h"
#include "Domain/Printer/HTNDomainPrinter.h"
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

    void RenderDecompositionByPlanningQuery(const std::vector<std::shared_ptr<const HTNMethodNode>>& inMethodNodes, HTNPlanningQuery& ioPlanningQuery,
                                            HTNDecompositionNode& ioSelectedNode);

    void ResetDecompositionPrinterState();

    HTN_NODISCARD bool IsLastWorldStateFileParsingOperationSuccessful() const;
    HTN_NODISCARD bool  IsLastDomainFileParsingOperationSuccessful() const;

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
    HTNOperationResult    mLastParseWorldStateFileOperationResult = HTNOperationResult::NONE;

    //----------------------------------------------------------------------//
    // Domain Printer
    //----------------------------------------------------------------------//
    std::filesystem::path mSelectedDomainFilePath;
    HTNOperationResult    mLastParseDomainFileOperationResult = HTNOperationResult::NONE;

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
#endif
