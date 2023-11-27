// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Domain/Interpreter/HTNDecompositionRecord.h"
#include "HTNCoreMinimal.h"

#include <memory>
#include <string>

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNDecompositionSnapshot.h"
#endif

class HTNDatabaseHook;
class HTNPlannerHook;

// Planning unit structure that holds the planner hook and the database
class HTNPlanningUnit
{
public:
    explicit HTNPlanningUnit(const HTNDatabaseHook& inDatabaseHook, const HTNPlannerHook& inPlannerHook,
                             const std::string& inDefaultTopLevelMethodID);

    // Executes the given top-level method of the domain
    bool ExecuteTopLevelMethod(const std::string& inEntryPointID);

    // Executes the default top-level method of the domain
    bool ExecuteTopLevelMethod();

    // Returns the database hook
    HTN_NODISCARD const HTNDatabaseHook& GetDatabaseHook() const;

    // Returns the planner hook
    HTN_NODISCARD const HTNPlannerHook& GetPlannerHook() const;

    // Returns the default top-level method ID
    HTN_NODISCARD const std::string& GetDefaultTopLevelMethodID() const;

    // Returns the last decomposition
    HTN_NODISCARD const HTNDecompositionRecord& GetLastDecomposition() const;

private:
    const HTNDatabaseHook& mDatabaseHook;
    const HTNPlannerHook&  mPlannerHook;
    std::string            mDefaultTopLevelMethodID;

    HTNDecompositionRecord mLastDecomposition;

#ifdef HTN_DEBUG_DECOMPOSITION
public:
    // Returns the snapshot of the last decomposition
    HTN_NODISCARD const HTNDecompositionSnapshot& GetLastDecompositionSnapshot() const;

private:
    HTNDecompositionSnapshot mLastDecompositionSnapshot;
#endif
};

inline const HTNDatabaseHook& HTNPlanningUnit::GetDatabaseHook() const
{
    return mDatabaseHook;
}

inline const HTNPlannerHook& HTNPlanningUnit::GetPlannerHook() const
{
    return mPlannerHook;
}

inline const std::string& HTNPlanningUnit::GetDefaultTopLevelMethodID() const
{
    return mDefaultTopLevelMethodID;
}

inline const HTNDecompositionRecord& HTNPlanningUnit::GetLastDecomposition() const
{
    return mLastDecomposition;
}

#ifdef HTN_DEBUG_DECOMPOSITION
inline const HTNDecompositionSnapshot& HTNPlanningUnit::GetLastDecompositionSnapshot() const
{
    return mLastDecompositionSnapshot;
}
#endif
