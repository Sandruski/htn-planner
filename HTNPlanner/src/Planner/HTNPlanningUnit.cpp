#include "HTNPlanningUnit.h"

#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"

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
