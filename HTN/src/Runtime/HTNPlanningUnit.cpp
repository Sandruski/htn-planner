#include "Runtime/HTNPlanningUnit.h"

#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNPlannerHook.h"

std::vector<HTNTaskInstance> HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName) const
{
    HTNDecompositionContext DecompositionContext = HTNDecompositionContext(mWorldState);
    return mPlanner->MakePlan(inEntryPointName, DecompositionContext);
}
