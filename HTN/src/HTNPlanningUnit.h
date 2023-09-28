#pragma once

#include <vector>
#include <string>

class HTNPlannerHook;
class HTNTaskResult;
class HTNWorldStateHook;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const HTNPlannerHook& inPlannerHook, const HTNWorldStateHook& inWorldStateHook);

    // Execute planning unit top level method
    bool ExecuteTopLevelMethod(const std::string& inEntryPointName, std::vector<HTNTaskResult>& outPlan) const;

private:
    const HTNPlannerHook*    mPlannerHook    = nullptr;
    const HTNWorldStateHook* mWorldStateHook = nullptr;
};

inline HTNPlanningUnit::HTNPlanningUnit(const HTNPlannerHook& inPlannerHook, const HTNWorldStateHook& inWorldStateHook)
    : mPlannerHook(&inPlannerHook), mWorldStateHook(&inWorldStateHook)
{
}
