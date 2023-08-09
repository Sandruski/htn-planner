#pragma once

#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNPlannerHook.h"
#include "Runtime/HTNWorldState.h"

#include <string>
#include <vector>

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    bool ParseDomain(const std::string& inDomainPath);

    HTNWorldState& GetWorldState()
    {
        return mWorldState;
    }

    // Execute planning unit top level method
    std::vector<HTNTaskInstance> ExecuteTopLevelMethod(const std::string& inEntryPointName);

private:
    HTNPlannerHook mPlanner;     // Decision making
    HTNWorldState  mWorldState;  // World state database
};

inline bool HTNPlanningUnit::ParseDomain(const std::string& inDomainPath)
{
    return mPlanner.parseDomain(inDomainPath);
}

inline std::vector<HTNTaskInstance> HTNPlanningUnit::ExecuteTopLevelMethod(const std::string& inEntryPointName)
{
    HTNDecompositionContext DecompositionContext = HTNDecompositionContext(mWorldState);
    return mPlanner.MakePlan(inEntryPointName, DecompositionContext);
}
