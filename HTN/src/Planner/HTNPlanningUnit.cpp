#include "HTNPlanningUnit.h"

#include "Domain/Interpreter/HTNTaskResult.h"
#include "Planner/HTNDatabaseHook.h"
#include "Planner/HTNPlannerHook.h"
#include "WorldState/HTNWorldState.h"

bool HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName, std::vector<HTNTaskResult>& outPlan) const
{
    const HTNWorldState& WorldState = mDatabaseHook->GetWorldState();
    return mPlannerHook->MakePlan(inEntryPointName, WorldState, outPlan);
}
