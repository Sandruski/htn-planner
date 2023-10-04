#pragma once

#include <string>
#include <vector>

class HTNDatabaseHook;
class HTNPlannerHook;
class HTNTaskResult;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const HTNPlannerHook& inPlannerHook, const HTNDatabaseHook& inDatabaseHook);

    // Execute planning unit top level method
    bool ExecuteTopLevelMethod(const std::string& inEntryPointName, std::vector<HTNTaskResult>& outPlan) const;

private:
    const HTNPlannerHook*  mPlannerHook  = nullptr;
    const HTNDatabaseHook* mDatabaseHook = nullptr;
};

inline HTNPlanningUnit::HTNPlanningUnit(const HTNPlannerHook& inPlannerHook, const HTNDatabaseHook& inDatabaseHook)
    : mPlannerHook(&inPlannerHook), mDatabaseHook(&inDatabaseHook)
{
}
