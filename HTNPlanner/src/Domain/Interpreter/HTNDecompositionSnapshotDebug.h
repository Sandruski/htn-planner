#pragma once

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNEnvironment.h"

#include <map>
#include <string>
#include <unordered_map>

class HTNNodeSnapshotDebug;

// Decomposition step to node snapshot
using HTNNodeSnapshotHistoryDebug = std::map<std::size_t, HTNNodeSnapshotDebug>;

// Node path to node snapshot history
using HTNNodeSnapshotHistoryCollectionDebug = std::unordered_map<std::string, HTNNodeSnapshotHistoryDebug>;

class HTNNodeSnapshotDebug
{
public:
    HTNNodeSnapshotDebug() = default;
    explicit HTNNodeSnapshotDebug(const bool inResult, const bool inIsChoicePoint, const HTNVariables& inVariables);

    bool                GetResult() const;
    bool                IsChoicePoint() const;
    const HTNVariables& GetVariables() const;

private:
    bool         mResult        = false;
    bool         mIsChoicePoint = false;
    HTNVariables mVariables;
};

class HTNDecompositionSnapshotDebug
{
public:
    void                               AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshotDebug& inNodeSnapshot);
    const HTNNodeSnapshotHistoryDebug* FindNodeSnapshotHistory(const std::string& inNodePath) const;

    void   IncrementDecompositionStep();
    size_t GetDecompositionStep() const;

private:
    HTNNodeSnapshotHistoryCollectionDebug mNodeSnapshotHistoryCollection;
    size_t                                mDecompositionStep = 0;
};

inline HTNNodeSnapshotDebug::HTNNodeSnapshotDebug(const bool inResult, const bool inIsChoicePoint, const HTNVariables& inVariables)
    : mResult(inResult), mIsChoicePoint(inIsChoicePoint), mVariables(inVariables)
{
}

inline bool HTNNodeSnapshotDebug::GetResult() const
{
    return mResult;
}

inline bool HTNNodeSnapshotDebug::IsChoicePoint() const
{
    return mIsChoicePoint;
}

inline const HTNVariables& HTNNodeSnapshotDebug::GetVariables() const
{
    return mVariables;
}

inline void HTNDecompositionSnapshotDebug::AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshotDebug& inNodeSnapshot)
{
    mNodeSnapshotHistoryCollection[inNodePath][mDecompositionStep] = inNodeSnapshot;
}

inline const HTNNodeSnapshotHistoryDebug* HTNDecompositionSnapshotDebug::FindNodeSnapshotHistory(const std::string& inNodePath) const
{
    const auto NodeSnapshotHistoryCollectionIt = mNodeSnapshotHistoryCollection.find(inNodePath);
    if (NodeSnapshotHistoryCollectionIt == mNodeSnapshotHistoryCollection.end())
    {
        return nullptr;
    }

    return &NodeSnapshotHistoryCollectionIt->second;
}

inline void HTNDecompositionSnapshotDebug::IncrementDecompositionStep()
{
    ++mDecompositionStep;
}

inline size_t HTNDecompositionSnapshotDebug::GetDecompositionStep() const
{
    return mDecompositionStep;
}
#endif
