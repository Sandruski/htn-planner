// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "HTNCoreMinimal.h"
#include "Window/HTNOperationResult.h"

#include <memory>
#include <string>

class HTNDomainNode;
class HTNPlanningUnit;

/**
 * Request of making a plan
 */
class HTNPlanningRequest
{
public:
    explicit HTNPlanningRequest(HTNPlanningUnit& ioPlanningUnit);

    // Returns the planning unit
    HTN_NODISCARD HTNPlanningUnit& GetPlanningUnitMutable();

    // Returns the planning unit
    HTN_NODISCARD const HTNPlanningUnit& GetPlanningUnit() const;

    // Sets the entry point ID to the given one
    void SetEntryPointID(const std::string& inEntryPointID);

    // Clears the entry point ID
    void ClearEntryPointID();

    // Returns the entry point ID
    HTN_NODISCARD const std::string& GetEntryPointID() const;

    // Returns whether the entry point ID is empty
    HTN_NODISCARD bool IsEntryPointIDEmpty() const;

    // Sets the last decompose domain opeartion result to the given one
    void SetLastDecomposeDomainOperationResult(const HTNOperationResult inOperationResult);

    // Returns the last decompose domain operation result
    HTN_NODISCARD HTNOperationResult GetLastDecomposeDomainOperationResult() const;

    // Returns whether the last decompose domain operation is successful
    HTN_NODISCARD bool IsLastDecomposeDomainOperationSuccessful() const;

    // Sets the last domain node to the given one
    void SetLastDomainNode(const std::shared_ptr<const HTNDomainNode>& inDomainNode);

    // Returns the last domain node
    HTN_NODISCARD const std::shared_ptr<const HTNDomainNode>& GetLastDomainNode() const;

    // Sets the last entry point ID to the given one
    void SetLastEntryPointID(const std::string& inEntryPointID);

    // Returns the last entry point ID
    HTN_NODISCARD const std::string& GetLastEntryPointID() const;

private:
    HTNPlanningUnit& mPlanningUnit;
    std::string      mEntryPointID;

    HTNOperationResult                   mLastDecomposeDomainOperationResult = HTNOperationResult::NONE;
    std::shared_ptr<const HTNDomainNode> mLastDomainNode;
    std::string                          mLastEntryPointID;
};

inline const HTNPlanningUnit& HTNPlanningRequest::GetPlanningUnit() const
{
    return mPlanningUnit;
}

inline HTNPlanningUnit& HTNPlanningRequest::GetPlanningUnitMutable()
{
    return mPlanningUnit;
}

inline void HTNPlanningRequest::SetEntryPointID(const std::string& inEntryPointID)
{
    mEntryPointID = inEntryPointID;
}

inline void HTNPlanningRequest::ClearEntryPointID()
{
    mEntryPointID.clear();
}

inline const std::string& HTNPlanningRequest::GetEntryPointID() const
{
    return mEntryPointID;
}

inline bool HTNPlanningRequest::IsEntryPointIDEmpty() const
{
    return mEntryPointID.empty();
}

inline void HTNPlanningRequest::SetLastDecomposeDomainOperationResult(const HTNOperationResult inOperationResult)
{
    mLastDecomposeDomainOperationResult = inOperationResult;
}

inline HTNOperationResult HTNPlanningRequest::GetLastDecomposeDomainOperationResult() const
{
    return mLastDecomposeDomainOperationResult;
}

inline void HTNPlanningRequest::SetLastDomainNode(const std::shared_ptr<const HTNDomainNode>& inDomainNode)
{
    mLastDomainNode = inDomainNode;
}

inline const std::shared_ptr<const HTNDomainNode>& HTNPlanningRequest::GetLastDomainNode() const
{
    return mLastDomainNode;
}

inline void HTNPlanningRequest::SetLastEntryPointID(const std::string& inEntryPointID)
{
    mLastEntryPointID = inEntryPointID;
}

inline const std::string& HTNPlanningRequest::GetLastEntryPointID() const
{
    return mLastEntryPointID;
}
#endif
