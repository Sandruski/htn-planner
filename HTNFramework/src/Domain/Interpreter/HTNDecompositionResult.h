// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNNodeResult.h"
#include "Domain/Interpreter/HTNNodeStep.h"
#include "HTNCoreMinimal.h"

#include <map>
#include <string>
#include <unordered_map>

class HTNNodeResultHistory;

// Node step to node result
using HTNNodeResultCollection = std::unordered_map<HTNNodeStep, HTNNodeResult>;

// Decomposition step to node result steps
using HTNNodeResultStepsCollection = std::map<size, HTNNodeResultCollection>;

// Node path to node result history
using HTNNodeResultHistoryCollection = std::unordered_map<std::string, HTNNodeResultHistory>;

class HTNNodeResultHistory
{
public:
    HTNNodeResultHistory() = default;
    explicit HTNNodeResultHistory(const bool inIsChoicePoint);

    HTN_NODISCARD const HTNNodeResultStepsCollection& GetNodeResultStepsCollection() const;
    HTN_NODISCARD HTNNodeResultStepsCollection&       GetNodeResultStepsCollectionMutable();
    HTN_NODISCARD bool                                IsChoicePoint() const;

private:
    HTNNodeResultStepsCollection mNodeResultStepsCollection;
    bool                         mIsChoicePoint = false;
};

class HTNDecompositionResult
{
public:
    void AddNodeResult(const std::string& inNodePath, const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);
    HTN_NODISCARD const HTNNodeResultHistory* FindNodeResultHistory(const std::string& inNodePath) const;

    void IncrementDecompositionStep();
    HTN_NODISCARD size GetDecompositionStep() const;

private:
    HTNNodeResultHistoryCollection mNodeResultHistoryCollection;
    size                           mDecompositionStep = 0;
};

inline const HTNNodeResultStepsCollection& HTNNodeResultHistory::GetNodeResultStepsCollection() const
{
    return mNodeResultStepsCollection;
}

inline HTNNodeResultStepsCollection& HTNNodeResultHistory::GetNodeResultStepsCollectionMutable()
{
    return mNodeResultStepsCollection;
}

inline bool HTNNodeResultHistory::IsChoicePoint() const
{
    return mIsChoicePoint;
}

inline const HTNNodeResultHistory* HTNDecompositionResult::FindNodeResultHistory(const std::string& inNodePath) const
{
    const auto NodeResultHistoryCollectionIt = mNodeResultHistoryCollection.find(inNodePath);
    if (NodeResultHistoryCollectionIt == mNodeResultHistoryCollection.end())
    {
        return nullptr;
    }

    return &NodeResultHistoryCollectionIt->second;
}

inline void HTNDecompositionResult::IncrementDecompositionStep()
{
    ++mDecompositionStep;
}

inline size HTNDecompositionResult::GetDecompositionStep() const
{
    return mDecompositionStep;
}
#endif
