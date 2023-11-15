#include "HTNPlanningUnit.h"

#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"

HTNPlanningUnit::HTNPlanningUnit(const std::string& inID, const HTNDatabaseHook& inDatabaseHook, const HTNPlannerHook& inPlannerHook)
    : mID(inID), mDatabaseHook(&inDatabaseHook), mPlannerHook(&inPlannerHook)
{
}

bool HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointID)
{
    const HTNWorldState&    WorldState           = mDatabaseHook->GetWorldState();
    HTNDecompositionContext DecompositionContext = HTNDecompositionContext(WorldState);
    const bool              Result               = mPlannerHook->MakePlan(inEntryPointID, DecompositionContext);

    mLastDecompositionContext = DecompositionContext;
    mLastEntryPointID         = inEntryPointID;
    mLastDomainNode           = mPlannerHook->GetDomainNode();

    return Result;
}
