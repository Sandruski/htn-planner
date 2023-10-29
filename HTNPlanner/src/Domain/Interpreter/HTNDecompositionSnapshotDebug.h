#pragma once

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNEnvironment.h"

#include <map>
#include <string>
#include <unordered_map>

class HTNNodeSnapshotDebug;
class HTNNodeSnapshotHistoryDebug;

// Decomposition step to node snapshot
using HTNNodeSnapshotCollectionDebug = std::map<std::size_t, HTNNodeSnapshotDebug>;

// Node path to node snapshot history
using HTNNodeSnapshotHistoryCollectionDebug = std::unordered_map<std::string, HTNNodeSnapshotHistoryDebug>;

class HTNNodeSnapshotDebug
{
public:
    HTNNodeSnapshotDebug() = default;
    explicit HTNNodeSnapshotDebug(const bool inResult, const HTNVariables& inVariables);

    bool                GetResult() const;
    const HTNVariables& GetVariables() const;

private:
    bool mResult = false;

    HTNVariables mVariables;
};

class HTNNodeSnapshotHistoryDebug
{
public:
    HTNNodeSnapshotHistoryDebug() = default;
    explicit HTNNodeSnapshotHistoryDebug(const bool inIsChoicePoint);

    const HTNNodeSnapshotCollectionDebug& GetNodeSnapshotCollection() const;
    HTNNodeSnapshotCollectionDebug&       GetNodeSnapshotCollectionMutable();
    bool                                  IsChoicePoint() const;

private:
    HTNNodeSnapshotCollectionDebug mNodeSnapshotCollection;
    bool                           mIsChoicePoint = false;
};

class HTNDecompositionSnapshotDebug
{
public:
    void AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshotDebug& inNodeSnapshot, const bool inIsChoicePoint);
    const HTNNodeSnapshotHistoryDebug* FindNodeSnapshotHistory(const std::string& inNodePath) const;

    void   IncrementDecompositionStep();
    size_t GetDecompositionStep() const;

private:
    HTNNodeSnapshotHistoryCollectionDebug mNodeSnapshotHistoryCollection;
    size_t                                mDecompositionStep = 0;
};

inline HTNNodeSnapshotDebug::HTNNodeSnapshotDebug(const bool inResult, const HTNVariables& inVariables) : mResult(inResult), mVariables(inVariables)
{
}

inline bool HTNNodeSnapshotDebug::GetResult() const
{
    return mResult;
}

inline const HTNVariables& HTNNodeSnapshotDebug::GetVariables() const
{
    return mVariables;
}

inline HTNNodeSnapshotHistoryDebug::HTNNodeSnapshotHistoryDebug(const bool inIsChoicePoint) : mIsChoicePoint(inIsChoicePoint)
{
}

inline const HTNNodeSnapshotCollectionDebug& HTNNodeSnapshotHistoryDebug::GetNodeSnapshotCollection() const
{
    return mNodeSnapshotCollection;
}

inline HTNNodeSnapshotCollectionDebug& HTNNodeSnapshotHistoryDebug::GetNodeSnapshotCollectionMutable()
{
    return mNodeSnapshotCollection;
}

inline bool HTNNodeSnapshotHistoryDebug::IsChoicePoint() const
{
    return mIsChoicePoint;
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
