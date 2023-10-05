#pragma once

#include "Domain/Interpreter/HTNDecompositionContext.h"

#include <string>

class HTNDatabaseHook;
class HTNPlannerHook;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const HTNPlannerHook& inPlannerHook, const HTNDatabaseHook& inDatabaseHook);

    // Execute planning unit top level method
    bool ExecuteTopLevelMethod(const std::string& inEntryPointName);

    const HTNDecompositionContext& GetLastDecompositionContext() const;

private:
    const HTNPlannerHook*  mPlannerHook  = nullptr;
    const HTNDatabaseHook* mDatabaseHook = nullptr;

    HTNDecompositionContext mLastDecompositionContext;
};

inline HTNPlanningUnit::HTNPlanningUnit(const HTNPlannerHook& inPlannerHook, const HTNDatabaseHook& inDatabaseHook)
    : mPlannerHook(&inPlannerHook), mDatabaseHook(&inDatabaseHook)
{
}

inline const HTNDecompositionContext& HTNPlanningUnit::GetLastDecompositionContext() const
{
    return mLastDecompositionContext;
}
