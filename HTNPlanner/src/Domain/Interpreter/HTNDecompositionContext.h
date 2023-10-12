#pragma once

#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Interpreter/HTNTaskResult.h"

#include <string>
#include <vector>

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#endif

class HTNWorldState;

class HTNDecompositionRecord
{
public:
    void            PushPendingTaskInstance(const HTNTaskInstance& inPendingTaskInstance);
    HTNTaskInstance PopPendingTaskInstance();
    bool            HasPendingTaskInstances() const;

    void                  SetEnvironment(const HTNEnvironment& inEnvironment);
    const HTNEnvironment& GetEnvironment() const;
    HTNEnvironment&       GetEnvironmentMutable();

    void                              AddTaskResultToPlan(const HTNTaskResult& inTaskResult);
    const std::vector<HTNTaskResult>& GetPlan() const;

private:
    // State of variables and indices
    HTNEnvironment mEnvironment;

    // Tasks pending to be processed
    std::vector<HTNTaskInstance> mPendingTaskInstances;

    // Final plan of tasks
    std::vector<HTNTaskResult> mPlan;
};

class HTNDecompositionContext
{
public:
    HTNDecompositionContext() = default;
    HTNDecompositionContext(const HTNWorldState& inWorldState);

    const HTNWorldState* GetWorldState() const;

    void RecordDecomposition(HTNDecompositionRecord& inDecomposition);
    bool RestoreDecomposition();

    const HTNDecompositionRecord& GetCurrentDecomposition() const;
    HTNDecompositionRecord&       GetCurrentDecompositionMutable();

    void                                       SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory);
    const std::vector<HTNDecompositionRecord>& GetDecompositionHistory() const;

    bool               TryPushNodeToCurrentNodePath(const std::string& inNodeID);
    bool               TryPopNodeFromCurrentNodePath();
    void               SetCurrentNodePath(const std::string& inCurrentNodePath);
    const std::string& GetCurrentNodePath() const;

    bool               TryPushNodeToCurrentVariableScopePath(const std::string& inNodeID);
    bool               TryPopNodeFromCurrentVariableScopePath();
    void               SetCurrentVariableScopePath(const std::string& inCurrentVariableScopePath);
    const std::string& GetCurrentVariableScopePath() const;
    bool               MakeCurrentVariablePath(const std::string& inVariableID, std::string& outVariablePath) const;

private:
    // TODO salvarez Maybe move the world state to the interpreter too
    const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at
                                                ///< all, this is why it is important this is a const pointer.

    // Current decomposition
    HTNDecompositionRecord mCurrentDecomposition;

    // Record of previous decompositions
    std::vector<HTNDecompositionRecord> mDecompositionHistory;

    // Path from the root node to the current node being processed
    std::string mCurrentNodePath;

    // Path from the root node to the node that determines the scope of variables
    std::string mCurrentVariableScopePath;

#ifdef HTN_DEBUG
public:
    void RecordNodeSnapshot(const std::string& inNodePath);

    const HTNDecompositionSnapshotDebug& GetDecompositionSnapshot() const;

private:
    HTNDecompositionSnapshotDebug mDecompositionSnapshot;
#endif
};

inline void HTNDecompositionRecord::PushPendingTaskInstance(const HTNTaskInstance& inPendingTaskInstance)
{
    mPendingTaskInstances.emplace_back(inPendingTaskInstance);
}

inline HTNTaskInstance HTNDecompositionRecord::PopPendingTaskInstance()
{
    const HTNTaskInstance PendingTaskInstance = mPendingTaskInstances.back();
    mPendingTaskInstances.pop_back();

    return PendingTaskInstance;
}

inline bool HTNDecompositionRecord::HasPendingTaskInstances() const
{
    return !mPendingTaskInstances.empty();
}

inline void HTNDecompositionRecord::SetEnvironment(const HTNEnvironment& inEnvironment)
{
    mEnvironment = inEnvironment;
}

inline const HTNEnvironment& HTNDecompositionRecord::GetEnvironment() const
{
    return mEnvironment;
}

inline HTNEnvironment& HTNDecompositionRecord::GetEnvironmentMutable()
{
    return mEnvironment;
}

inline void HTNDecompositionRecord::AddTaskResultToPlan(const HTNTaskResult& inTaskResult)
{
    mPlan.emplace_back(inTaskResult);
}

inline const std::vector<HTNTaskResult>& HTNDecompositionRecord::GetPlan() const
{
    return mPlan;
}

inline HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState) : mWorldState(&inWorldState)
{
}

inline const HTNWorldState* HTNDecompositionContext::GetWorldState() const
{
    return mWorldState;
}

inline void HTNDecompositionContext::RecordDecomposition(HTNDecompositionRecord& inDecomposition)
{
    mDecompositionHistory.emplace_back(inDecomposition);
}

inline const HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecomposition() const
{
    return mCurrentDecomposition;
}

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
    return mCurrentDecomposition;
}

inline void HTNDecompositionContext::SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory)
{
    mDecompositionHistory = inDecompositionHistory;
}

inline const std::vector<HTNDecompositionRecord>& HTNDecompositionContext::GetDecompositionHistory() const
{
    return mDecompositionHistory;
}

inline void HTNDecompositionContext::SetCurrentNodePath(const std::string& inCurrentNodePath)
{
    mCurrentNodePath = inCurrentNodePath;
}

inline const std::string& HTNDecompositionContext::GetCurrentNodePath() const
{
    return mCurrentNodePath;
}

inline void HTNDecompositionContext::SetCurrentVariableScopePath(const std::string& inCurrentVariableScopePath)
{
    mCurrentVariableScopePath = inCurrentVariableScopePath;
}

inline const std::string& HTNDecompositionContext::GetCurrentVariableScopePath() const
{
    return mCurrentVariableScopePath;
}

inline const HTNDecompositionSnapshotDebug& HTNDecompositionContext::GetDecompositionSnapshot() const
{
    return mDecompositionSnapshot;
}
