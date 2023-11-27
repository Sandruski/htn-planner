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

// Decomposition step to node result collection
using HTNNodeResultStepsCollection = std::map<size, HTNNodeResultCollection>;

// Node path to node result history
using HTNNodeResultHistoryCollection = std::unordered_map<std::string, HTNNodeResultHistory>;

/**
 * Previous node results of a decomposition
 */
class HTNNodeResultHistory
{
public:
    HTNNodeResultHistory() = default;
    explicit HTNNodeResultHistory(const bool inIsChoicePoint);

    // Returns the node result steps collection
    HTN_NODISCARD const HTNNodeResultStepsCollection& GetNodeResultStepsCollection() const;

    // Returns the node result steps collection
    HTN_NODISCARD HTNNodeResultStepsCollection& GetNodeResultStepsCollectionMutable();

    // Returns whether the node is a choice point
    HTN_NODISCARD bool IsChoicePoint() const;

private:
    HTNNodeResultStepsCollection mNodeResultStepsCollection;

    // Whether the node is a choice point
    bool mIsChoicePoint = false;
};

/**
 * Result of a decomposition
 */
class HTNDecompositionResult
{
public:
    // Adds the given node result to the history collection
    void AddNodeResult(const std::string& inNodePath, const HTNNodeResult& inNodeResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);

    // Returns the node result history at the given node path
    HTN_NODISCARD const HTNNodeResultHistory* FindNodeResultHistory(const std::string& inNodePath) const;

    // Increments the current decomposition step
    void IncrementCurrentDecompositionStep();

    // Returns the current decomposition step
    HTN_NODISCARD size GetCurrentDecompositionStep() const;

private:
    HTNNodeResultHistoryCollection mNodeResultHistoryCollection;

    // Number of times the algorithm has backtracked
    size mCurrentDecompositionStep = 0;
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

inline void HTNDecompositionResult::IncrementCurrentDecompositionStep()
{
    ++mCurrentDecompositionStep;
}

inline size HTNDecompositionResult::GetCurrentDecompositionStep() const
{
    return mCurrentDecompositionStep;
}
#endif
