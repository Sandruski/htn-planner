// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Helpers/HTNOperationResult.h"

#include <string>
#include <memory>

class HTNDomainNode;
class HTNPlanningUnit;

class HTNPlanningQuery
{
public:
    explicit HTNPlanningQuery(HTNPlanningUnit& inPlanningUnit);

    HTNPlanningUnit&       GetPlanningUnitMutable();
    const HTNPlanningUnit& GetPlanningUnit() const;

    void               SetEntryPointID(const std::string& inEntryPointID);
    void               ClearEntryPointID();
    const std::string& GetEntryPointID() const;
    bool               IsEntryPointIDEmpty() const;

    void               SetLastDecompositionResult(const HTNOperationResult inLastDecompositionResult);
    HTNOperationResult GetLastDecompositionResult() const;
    bool               IsLastDecompositionSuccessful() const;

    void                                        SetLastDomainNode(const std::shared_ptr<const HTNDomainNode>& inLastDomainNode);
    const std::shared_ptr<const HTNDomainNode>& GetLastDomainNode() const;
    void                                        SetLastEntryPointID(const std::string& inLastEntryPointID);
    const std::string&                          GetLastEntryPointID() const;

private:
    HTNPlanningUnit& mPlanningUnit;
    std::string      mEntryPointID;

    HTNOperationResult                   mLastDecompositionResult = HTNOperationResult::NONE;
    std::shared_ptr<const HTNDomainNode> mLastDomainNode;
    std::string                          mLastEntryPointID;
};

inline const HTNPlanningUnit& HTNPlanningQuery::GetPlanningUnit() const
{
    return mPlanningUnit;
}

inline HTNPlanningUnit& HTNPlanningQuery::GetPlanningUnitMutable()
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

inline void HTNPlanningQuery::SetLastDomainNode(const std::shared_ptr<const HTNDomainNode>& inLastDomainNode)
{
    mLastDomainNode = inLastDomainNode;
}

inline const std::shared_ptr<const HTNDomainNode>& HTNPlanningQuery::GetLastDomainNode() const
{
    return mLastDomainNode;
}

inline void HTNPlanningQuery::SetLastEntryPointID(const std::string& inLastEntryPointID)
{
    mLastEntryPointID = inLastEntryPointID;
}

inline const std::string& HTNPlanningQuery::GetLastEntryPointID() const
{
    return mLastEntryPointID;
}
#endif
