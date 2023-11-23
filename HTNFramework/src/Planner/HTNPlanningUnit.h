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

    const std::string&     GetID() const;
    const HTNDatabaseHook& GetDatabaseHook() const;
    const HTNPlannerHook&  GetPlannerHook() const;

    const HTNDecompositionRecord& GetLastDecomposition() const;

private:
    std::string            mID;
    const HTNDatabaseHook& mDatabaseHook;
    const HTNPlannerHook&  mPlannerHook;

    HTNDecompositionRecord mLastDecomposition;

#ifdef HTN_DEBUG_DECOMPOSITION
public:
    const HTNDecompositionResult& GetLastDecompositionResult() const;

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
