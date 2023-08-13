#pragma once

#include "Runtime/HTNWorldState.h"

#include <string>
#include <vector>

class HTNPlannerHook;
class HTNTaskInstance;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const std::string& inName, const HTNPlannerHook& inPlanner);

    // Execute planning unit top level method
    std::vector<HTNTaskInstance> ExecuteTopLevelMethod(const std::string& inEntryPointName) const;

    const std::string& GetName() const;

    const HTNPlannerHook& GetPlanner() const;

    const HTNWorldState& GetWorldState() const;
    HTNWorldState&       GetWorldStateMutable();

private:
    std::string           mName;
    const HTNPlannerHook* mPlanner = nullptr; // Decision making
    HTNWorldState         mWorldState;        // World state database
};

inline HTNPlanningUnit::HTNPlanningUnit(const std::string& inName, const HTNPlannerHook& inPlanner) : mName(inName), mPlanner(&inPlanner)
{
}

inline const std::string& HTNPlanningUnit::GetName() const
{
    return mName;
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
