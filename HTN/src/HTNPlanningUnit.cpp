#include "HTNPlanningUnit.h"

#include "Interpreter/HTNDecompositionContext.h"
#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNTaskResult.h"

std::vector<HTNTaskResult> HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName) const
{
    HTNDecompositionContext DecompositionContext = HTNDecompositionContext(mWorldState);
    return mPlanner->MakePlan(inEntryPointName, DecompositionContext);
}
