#pragma once

#include "Runtime/HTNWorldState.h"

#include <vector>

class HTNPlannerHook;
class HTNTaskInstance;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const HTNPlannerHook& inPlanner, const HTNWorldState& inWorldState);

    // Execute planning unit top level method
    std::vector<HTNTaskInstance> ExecuteTopLevelMethod(const std::string& inEntryPointName) const;

    const HTNPlannerHook& GetPlanner() const;

    const HTNWorldState& GetWorldState() const;
    HTNWorldState&       GetWorldStateMutable();

private:
    const HTNPlannerHook* mPlanner = nullptr; // Decision making
    HTNWorldState         mWorldState;        // World state database
};

inline HTNPlanningUnit::HTNPlanningUnit(const HTNPlannerHook& inPlanner, const HTNWorldState& inWorldState)
    : mPlanner(&inPlanner), mWorldState(inWorldState)
{
}

inline const HTNPlannerHook& HTNPlanningUnit::GetPlanner() const
{
    return *mPlanner;
}

inline const HTNWorldState& HTNPlanningUnit::GetWorldState() const
{
    return mWorldState;
}

inline HTNWorldState& HTNPlanningUnit::GetWorldStateMutable()
{
    return mWorldState;
}
