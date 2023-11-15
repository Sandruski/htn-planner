#pragma once

#include "Domain/Interpreter/HTNDecompositionContext.h"

#include <memory>
#include <string>

class HTNDatabaseHook;
class HTNDomainNode;
class HTNPlannerHook;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const std::string& inID, const HTNDatabaseHook& inDatabaseHook, const HTNPlannerHook& inPlannerHook);

    // Execute planning unit top level method
    bool ExecuteTopLevelMethod(const std::string& inEntryPointID);

    const std::string&                          GetID() const;
    const HTNDecompositionContext&              GetLastDecompositionContext() const;
    const std::string&                          GetLastEntryPointID() const;
    const std::shared_ptr<const HTNDomainNode>& GetLastDomainNode() const;

private:
    std::string            mID;
    const HTNDatabaseHook* mDatabaseHook = nullptr;
    const HTNPlannerHook*        mPlannerHook  = nullptr;

    HTNDecompositionContext              mLastDecompositionContext;
    std::string                          mLastEntryPointID;
    std::shared_ptr<const HTNDomainNode> mLastDomainNode;
};

inline const std::string& HTNPlanningUnit::GetID() const
{
    return mID;
}

inline const HTNDecompositionContext& HTNPlanningUnit::GetLastDecompositionContext() const
{
    return mLastDecompositionContext;
}

inline const std::string& HTNPlanningUnit::GetLastEntryPointID() const
{
    return mLastEntryPointID;
}

inline const std::shared_ptr<const HTNDomainNode>& HTNPlanningUnit::GetLastDomainNode() const
{
    return mLastDomainNode;
}
