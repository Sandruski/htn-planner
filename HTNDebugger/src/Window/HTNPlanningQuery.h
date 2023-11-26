// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Window/HTNOperationResult.h"

#include <memory>
#include <string>

class HTNDomainNode;
class HTNPlanningUnit;

class HTNPlanningQuery
{
public:
    explicit HTNPlanningQuery(HTNPlanningUnit& ioPlanningUnit);

    HTN_NODISCARD HTNPlanningUnit&       GetPlanningUnitMutable();
    HTN_NODISCARD const HTNPlanningUnit& GetPlanningUnit() const;

    void                SetEntryPointID(const std::string& inEntryPointID);
    void                ClearEntryPointID();
    HTN_NODISCARD const std::string& GetEntryPointID() const;
    HTN_NODISCARD bool               IsEntryPointIDEmpty() const;

    void                             SetLastDecomposeDomainOperationResult(const HTNOperationResult inLastDecomposeDomainOperationResult);
    HTN_NODISCARD HTNOperationResult GetLastDecomposeDomainOperationResult() const;
    HTN_NODISCARD bool               IsLastDecomposeDomainOperationSuccessful() const;

    void                SetLastDomainNode(const std::shared_ptr<const HTNDomainNode>& inLastDomainNode);
    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetLastDomainNode() const;
    void                                                      SetLastEntryPointID(const std::string& inLastEntryPointID);
    HTN_NODISCARD const std::string& GetLastEntryPointID() const;

private:
    HTNPlanningUnit& mPlanningUnit;
    std::string      mEntryPointID;

    HTNOperationResult                   mLastDecomposeDomainOperationResult = HTNOperationResult::NONE;
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

inline void HTNPlanningQuery::SetLastDecomposeDomainOperationResult(const HTNOperationResult inLastDecomposeDomainOperationResult)
{
    mLastDecomposeDomainOperationResult = inLastDecomposeDomainOperationResult;
}

inline HTNOperationResult HTNPlanningQuery::GetLastDecomposeDomainOperationResult() const
{
    return mLastDecomposeDomainOperationResult;
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
