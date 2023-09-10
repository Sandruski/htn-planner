#include "Runtime/HTNPlanningUnit.h"

#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNPlannerHook.h"
#include "Runtime/HTNTaskResult.h"

std::vector<HTNTaskResult> HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName) const
{
    HTNDecompositionContext DecompositionContext = HTNDecompositionContext(mWorldState);
    return mPlanner->MakePlan(inEntryPointName, DecompositionContext);
}
