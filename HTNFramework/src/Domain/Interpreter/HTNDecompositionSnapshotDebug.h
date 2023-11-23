// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#ifdef HTN_DEBUG_DECOMPOSITION
#include "Domain/Interpreter/HTNEnvironment.h"
#include "HTNCoreMinimal.h"

#include <map>
#include <string>
#include <unordered_map>

class HTNNodeSnapshotDebug;
class HTNNodeSnapshotHistoryDebug;

enum class HTNNodeStep : uint8
{
    START,
    END, // RETRY
    NONE
};

// Node step to node snapshot
using HTNNodeSnapshotCollectionDebug = std::unordered_map<HTNNodeStep, HTNNodeSnapshotDebug>;

// Decomposition step to node snapshot steps
using HTNNodeSnapshotStepsCollectionDebug = std::map<size, HTNNodeSnapshotCollectionDebug>;

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
    bool                                       IsChoicePoint() const;

private:
    HTNNodeSnapshotStepsCollectionDebug mNodeSnapshotStepsCollection;
    bool                                mIsChoicePoint = false;
};

class HTNDecompositionSnapshotDebug
{
public:
    void AddNodeSnapshot(const std::string& inNodePath, const HTNNodeSnapshotDebug& inNodeSnapshot, const HTNNodeStep inNodeStep,
                         const bool inIsChoicePoint);
    const HTNNodeSnapshotHistoryDebug* FindNodeSnapshotHistory(const std::string& inNodePath) const;

    void IncrementDecompositionStep();
    size GetDecompositionStep() const;

private:
    HTNNodeSnapshotHistoryCollectionDebug mNodeSnapshotHistoryCollection;
    size                                  mDecompositionStep = 0;
};

inline bool HTNNodeSnapshotDebug::GetResult() const
{
    return mResult;
}

inline const HTNVariables& HTNNodeSnapshotDebug::GetVariables() const
{
    return mVariables;
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

inline size HTNDecompositionSnapshotDebug::GetDecompositionStep() const
{
    return mDecompositionStep;
}
#endif
