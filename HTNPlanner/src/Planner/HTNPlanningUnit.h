#pragma once

#include "HTNPlannerMinimal.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"

#include <memory>
#include <string>

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#endif

class HTNDatabaseHook;
class HTNPlannerHook;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const std::string& inID, const HTNDatabaseHook& inDatabaseHook, const HTNPlannerHook& inPlannerHook);

    // Execute planning unit top level method
    bool ExecuteTopLevelMethod(const std::string& inEntryPointID);

    const std::string&     GetID() const;
    const HTNDatabaseHook& GetDatabaseHook() const;
    const HTNPlannerHook&  GetPlannerHook() const;

    const HTNDecompositionRecord& GetLastDecomposition() const;

private:
    std::string            mID;
    const HTNDatabaseHook& mDatabaseHook;
    const HTNPlannerHook&  mPlannerHook;

    HTNDecompositionRecord mLastDecomposition;

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

inline const HTNDatabaseHook& HTNPlanningUnit::GetDatabaseHook() const
{
    return mDatabaseHook;
}

inline const HTNPlannerHook& HTNPlanningUnit::GetPlannerHook() const
{
    return mPlannerHook;
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
