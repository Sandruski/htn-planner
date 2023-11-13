#pragma once

#ifdef HTN_DEBUG
#include "Helpers/HTNDebuggerWindowHelpers.h"

#include <string>

class HTNPlanningUnit;

class HTNPlanningQuery
{
public:
    explicit HTNPlanningQuery(HTNPlanningUnit* inPlanningUnit);

    HTNPlanningUnit*       GetPlanningUnitMutable();
    const HTNPlanningUnit* GetPlanningUnit() const;
    void                   SetEntryPointID(const std::string& inEntryPointID);
    void                   ClearEntryPointID();
    const std::string&     GetEntryPointID() const;
    bool                   IsEntryPointIDEmpty() const;
    void                   SetLastDecompositionResult(const HTNOperationResult inLastDecompositionResult);
    HTNOperationResult     GetLastDecompositionResult() const;
    bool                   IsLastDecompositionSuccessful() const;

private:
    HTNPlanningUnit*   mPlanningUnit = nullptr;
    std::string        mEntryPointID;
    HTNOperationResult mLastDecompositionResult = HTNOperationResult::NONE;
};

inline const HTNPlanningUnit* HTNPlanningQuery::GetPlanningUnit() const
{
    return mPlanningUnit;
}

inline HTNPlanningUnit* HTNPlanningQuery::GetPlanningUnitMutable()
{
    return mPlanningUnit;
}

inline void HTNPlanningQuery::SetEntryPointID(const std::string& inEntryPointID)
{
    mEntryPointID = inEntryPointID;
}

inline void HTNPlanningQuery::ClearEntryPointID()
{
    mEntryPointID.clear();
}

inline const std::string& HTNPlanningQuery::GetEntryPointID() const
{
    return mEntryPointID;
}

inline bool HTNPlanningQuery::IsEntryPointIDEmpty() const
{
    return mEntryPointID.empty();
}

inline void HTNPlanningQuery::SetLastDecompositionResult(const HTNOperationResult inLastDecompositionResult)
{
    mLastDecompositionResult = inLastDecompositionResult;
}

inline HTNOperationResult HTNPlanningQuery::GetLastDecompositionResult() const
{
    return mLastDecompositionResult;
}

inline bool HTNPlanningQuery::IsLastDecompositionSuccessful() const
{
    return HTNDebuggerWindowHelpers::IsOperationSuccessful(mLastDecompositionResult);
}
#endif
