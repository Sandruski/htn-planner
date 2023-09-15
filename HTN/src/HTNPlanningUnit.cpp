#include "HTNPlanningUnit.h"

#include "Planner/HTNPlannerHook.h"
#include "Planner/HTNTaskResult.h"

std::vector<HTNTaskResult> HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName) const
{
    return mPlanner->MakePlan(inEntryPointName, mWorldState);
}
