#pragma once

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNEnvironment.h"

#include <map>
#include <string>
#include <unordered_map>

class HTNNodeSnapshotDebug;
class HTNNodeSnapshotHistoryDebug;

enum class HTNNodeStep : unsigned char
{
    START,
    END, // RETRY
    NONE
};

// Node step to node snapshot
using HTNNodeSnapshotCollectionDebug = std::unordered_map<HTNNodeStep, HTNNodeSnapshotDebug>;

// Decomposition step to node snapshot steps
using HTNNodeSnapshotStepsCollectionDebug = std::map<std::size_t, HTNNodeSnapshotCollectionDebug>;

// Node path to node snapshot history
using HTNNodeSnapshotHistoryCollectionDebug = std::unordered_map<std::string, HTNNodeSnapshotHistoryDebug>;

class HTNNodeSnapshotDebug
{
public:
    HTNNodeSnapshotDebug() = default;
    explicit HTNNodeSnapshotDebug(const HTNVariables& inVariables);
    explicit HTNNodeSnapshotDebug(const HTNVariables& inVariables, const bool inResult);

    const HTNVariables& GetVariables() const;
    bool                GetResult() const;

private:
    HTNVariables mVariables;
    bool         mResult = false;
};

class HTNNodeSnapshotHistoryDebug
{
public:
    HTNNodeSnapshotHistoryDebug() = default;
    explicit HTNNodeSnapshotHistoryDebug(const bool inIsChoicePoint);

    const HTNNodeSnapshotStepsCollectionDebug& GetNodeSnapshotStepsCollection() const;
    HTNNodeSnapshotStepsCollectionDebug&       GetNodeSnapshotStepsCollectionMutable();
    bool                                      IsChoicePoint() const;

private:
    HTNNodeSnapshotStepsCollectionDebug mNodeSnapshotStepsCollection;
    bool                               mIsChoicePoint = false;
};

class HTNDecompositionSnapshotDebug
{
public:
    void AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep,
                         const bool inIsChoicePoint);
    const HTNNodeSnapshotHistoryDebug* FindNodeSnapshotHistory(const std::string& inNodePath) const;

    void   IncrementDecompositionStep();
    size_t GetDecompositionStep() const;

private:
    HTNNodeSnapshotHistoryCollectionDebug mNodeSnapshotHistoryCollection;
    size_t                                mDecompositionStep = 0;
};

inline HTNNodeSnapshotDebug::HTNNodeSnapshotDebug(const HTNVariables& inVariables) : mVariables(inVariables)
{
}

inline HTNNodeSnapshotDebug::HTNNodeSnapshotDebug(const HTNVariables& inVariables, const bool inResult) : mVariables(inVariables), mResult(inResult)
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

inline const HTNNodeSnapshotStepsCollectionDebug& HTNNodeSnapshotHistoryDebug::GetNodeSnapshotStepsCollection() const
{
    return mNodeSnapshotStepsCollection;
}

inline HTNNodeSnapshotStepsCollectionDebug& HTNNodeSnapshotHistoryDebug::GetNodeSnapshotStepsCollectionMutable()
{
    return mNodeSnapshotStepsCollection;
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
