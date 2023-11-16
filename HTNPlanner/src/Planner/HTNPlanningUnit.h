#pragma once

#include "Domain/Interpreter/HTNDecompositionRecord.h"

#include <memory>
#include <string>

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#endif

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

    const std::string& GetID() const;

    const std::shared_ptr<const HTNDomainNode>& GetLastDomainNode() const;
    const std::string&                          GetLastEntryPointID() const;
    const HTNDecompositionRecord&               GetLastDecomposition() const;

private:
    std::string            mID;
    const HTNDatabaseHook* mDatabaseHook = nullptr;
    const HTNPlannerHook*  mPlannerHook  = nullptr;

    std::shared_ptr<const HTNDomainNode> mLastDomainNode;
    std::string                          mLastEntryPointID;
    HTNDecompositionRecord               mLastDecomposition;

#ifdef HTN_DEBUG
public:
    const HTNDecompositionSnapshotDebug& GetLastDecompositionSnapshot() const;

private:
    HTNDecompositionSnapshotDebug mLastDecompositionSnapshot;
#endif
};

inline const std::string& HTNPlanningUnit::GetID() const
{
    return mID;
}

inline const std::shared_ptr<const HTNDomainNode>& HTNPlanningUnit::GetLastDomainNode() const
{
    return mLastDomainNode;
}

inline const std::string& HTNPlanningUnit::GetLastEntryPointID() const
{
    return mLastEntryPointID;
}

inline const HTNDecompositionRecord& HTNPlanningUnit::GetLastDecomposition() const
{
    return mLastDecomposition;
}

#ifdef HTN_DEBUG
inline const HTNDecompositionSnapshotDebug& HTNPlanningUnit::GetLastDecompositionSnapshot() const
{
    return mLastDecompositionSnapshot;
}
#endif
