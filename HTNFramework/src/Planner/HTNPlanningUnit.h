// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Domain/Interpreter/HTNDecompositionRecord.h"

#include <memory>
#include <string>

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNDecompositionResult.h"
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

    HTN_NODISCARD const std::string&     GetID() const;
    HTN_NODISCARD const HTNDatabaseHook& GetDatabaseHook() const;
    HTN_NODISCARD const HTNPlannerHook&  GetPlannerHook() const;

    HTN_NODISCARD const HTNDecompositionRecord& GetLastDecomposition() const;

private:
    std::string            mID;
    const HTNDatabaseHook& mDatabaseHook;
    const HTNPlannerHook&  mPlannerHook;

    HTNDecompositionRecord mLastDecomposition;

#ifdef HTN_DEBUG_DECOMPOSITION
public:
    HTN_NODISCARD const HTNDecompositionResult& GetLastDecompositionResult() const;

private:
    HTNDecompositionResult mLastDecompositionResult;
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

#ifdef HTN_DEBUG_DECOMPOSITION
inline const HTNDecompositionResult& HTNPlanningUnit::GetLastDecompositionResult() const
{
    return mLastDecompositionResult;
}
#endif
