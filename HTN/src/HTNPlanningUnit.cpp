#include "HTNPlanningUnit.h"

#include "WorldState/HTNWorldState.h"
#include "WorldState/HTNWorldStateHook.h"
#include "Interpreter/HTNTaskResult.h"
#include "Planner/HTNPlannerHook.h"

bool HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName, std::vector<HTNTaskResult>& outPlan) const
{
    // Copy the world state
    const HTNWorldState WorldState = mWorldStateHook->GetWorldState();
    return mPlannerHook->MakePlan(inEntryPointName, WorldState, outPlan);
}
