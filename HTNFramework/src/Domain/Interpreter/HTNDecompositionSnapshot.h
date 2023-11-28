// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNNodeSnapshot.h"
#include "Domain/Interpreter/HTNNodeStep.h"
#include "HTNCoreMinimal.h"

#include <map>
#include <string>
#include <unordered_map>

class HTNNodeSnapshotHistory;

// Node step to node snapshot
using HTNNodeSnapshotCollection = std::unordered_map<HTNNodeStep, HTNNodeSnapshot>;

// Decomposition step to node snapshot collection
using HTNNodeSnapshotStepsCollection = std::map<size, HTNNodeSnapshotCollection>;

// Node path to node snapshot history
using HTNNodeSnapshotHistoryCollection = std::unordered_map<std::string, HTNNodeSnapshotHistory>;

/**
 * Previous node snapshots of a decomposition
 */
class HTNNodeSnapshotHistory
{
public:
    HTNNodeSnapshotHistory() = default;
    explicit HTNNodeSnapshotHistory(const bool inIsChoicePoint);

    // Returns the node snapshot steps collection
    HTN_NODISCARD const HTNNodeSnapshotStepsCollection& GetNodeSnapshotStepsCollection() const;

    // Returns the node snapshot steps collection
    HTN_NODISCARD HTNNodeSnapshotStepsCollection& GetNodeSnapshotStepsCollectionMutable();

    // Returns whether the node is a choice point
    HTN_NODISCARD bool IsChoicePoint() const;

private:
    HTNNodeSnapshotStepsCollection mNodeSnapshotStepsCollection;

    // Whether the node is a choice point
    bool mIsChoicePoint = false;
};

/**
 * Snapshot of a decomposition
 */
class HTNDecompositionSnapshot
{
public:
    // Adds the given node snapshot to the history collection
    void AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshot& inNodeSnapshot, const HTNNodeStep inNodeStep,
                         const bool inIsChoicePoint);

    // Returns the node snapshot history at the given node path
    HTN_NODISCARD const HTNNodeSnapshotHistory* FindNodeSnapshotHistory(const std::string& inNodePath) const;

    // Increments the current decomposition step
    void IncrementCurrentDecompositionStep();

    // Returns the current decomposition step
    HTN_NODISCARD size GetCurrentDecompositionStep() const;

private:
    HTNNodeSnapshotHistoryCollection mNodeSnapshotHistoryCollection;

    // Number of times the algorithm has backtracked
    size mCurrentDecompositionStep = 0;
};

inline const HTNNodeSnapshotStepsCollection& HTNNodeSnapshotHistory::GetNodeSnapshotStepsCollection() const
{
    return mNodeSnapshotStepsCollection;
}

inline HTNNodeSnapshotStepsCollection& HTNNodeSnapshotHistory::GetNodeSnapshotStepsCollectionMutable()
{
    return mNodeSnapshotStepsCollection;
}

inline bool HTNNodeSnapshotHistory::IsChoicePoint() const
{
    return mIsChoicePoint;
}

inline const HTNNodeSnapshotHistory* HTNDecompositionSnapshot::FindNodeSnapshotHistory(const std::string& inNodePath) const
{
    const auto NodeSnapshotHistoryCollectionIt = mNodeSnapshotHistoryCollection.find(inNodePath);
    if (NodeSnapshotHistoryCollectionIt == mNodeSnapshotHistoryCollection.cend())
    {
        return nullptr;
    }

    return &NodeSnapshotHistoryCollectionIt->second;
}

inline void HTNDecompositionSnapshot::IncrementCurrentDecompositionStep()
{
    ++mCurrentDecompositionStep;
}

inline size HTNDecompositionSnapshot::GetCurrentDecompositionStep() const
{
    return mCurrentDecompositionStep;
}
#endif
