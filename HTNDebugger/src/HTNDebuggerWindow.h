#pragma once

#ifdef HTN_DEBUG
#include "Domain/HTNDecompositionPrinter.h"
#include "Domain/HTNDecompositionWatchWindowPrinter.h"

#include <memory>
#include <vector>

class HTNDatabaseHook;
class HTNDecompositionNode;
class HTNMethodNode;
class HTNPlannerHook;
class HTNPlanningUnit;

enum class HTNOperationResult : unsigned char
{
    FAILED    = 0,
    SUCCEEDED = 1,
    NONE,
};

// TODO salvarez Make it a class and define the methods somewhere else
struct HTNPlanningQuery
{
    explicit HTNPlanningQuery(HTNPlanningUnit* inPlanningUnit) : mPlanningUnit(inPlanningUnit)
    {
    }

    HTNPlanningUnit*   mPlanningUnit = nullptr;
    std::string        mEntryPointID;
    HTNOperationResult mLastDecompositionResult = HTNOperationResult::NONE;
};

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

    HTNDatabaseHook* mDatabaseHook          = nullptr;
    HTNPlannerHook*  mPlannerHook           = nullptr;
    HTNPlanningUnit* mMainPlanningUnit      = nullptr;
    HTNPlanningUnit* mUpperBodyPlanningUnit = nullptr;

    HTNDecompositionPrinter            mDecompositionPrinter;
    HTNDecompositionWatchWindowPrinter mDecompositionWatchWindowPrinter;
};
#endif
