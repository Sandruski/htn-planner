#include "HTNPlanningUnit.h"

#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"

bool HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName)
{
    const HTNWorldState&    WorldState           = mDatabaseHook->GetWorldState();
    HTNDecompositionContext DecompositionContext = HTNDecompositionContext(WorldState);
    const bool              Result               = mPlannerHook->MakePlan(inEntryPointName, DecompositionContext);
    if (Result)
    {
        mLastDecompositionContext = DecompositionContext;
    }

    return Result;
}
