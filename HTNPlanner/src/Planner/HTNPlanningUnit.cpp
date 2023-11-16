#include "HTNPlanningUnit.h"

#include "Domain/Interpreter/HTNDecompositionContext.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"

HTNPlanningUnit::HTNPlanningUnit(const std::string& inID, const HTNDatabaseHook& inDatabaseHook, const HTNPlannerHook& inPlannerHook)
    : mID(inID), mDatabaseHook(&inDatabaseHook), mPlannerHook(&inPlannerHook)
{
}

bool HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointID)
{
    mLastEntryPointID = inEntryPointID;
    mLastDomainNode   = mPlannerHook->GetDomainNode();

    const HTNWorldState&    WorldState           = mDatabaseHook->GetWorldState();
    HTNDecompositionContext DecompositionContext = HTNDecompositionContext(WorldState);
    const bool              Result               = mPlannerHook->MakePlan(inEntryPointID, DecompositionContext);

    mLastDecomposition = DecompositionContext.GetCurrentDecomposition();
#ifdef HTN_DEBUG
    mLastDecompositionSnapshot = DecompositionContext.GetDecompositionSnapshot();
#endif

    return Result;
}
