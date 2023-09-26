#pragma once

#include "HTNAtom.h"
#include "Interpreter/HTNEnvironment.h"
#include "Interpreter/HTNTaskInstance.h"
#include "Interpreter/HTNTaskResult.h"

#include <memory>
#include <vector>

class HTNCompoundTaskNode;
class HTNDomainNode;
class HTNMethodNode;
class HTNTaskNodeBase;
class HTNWorldState;
class HTNNodeBase;

class HTNDecompositionRecord
{
public:
    void            PushPendingTaskNode(const std::shared_ptr<const HTNTaskNodeBase>& inPendingTaskNode);
    HTNTaskInstance PopPendingTaskInstance();
    bool            HasPendingTaskInstances() const;

    void                                          SetCurrentTaskNode(const std::shared_ptr<const HTNTaskNodeBase>& inCurrentTaskNode);
    const std::shared_ptr<const HTNTaskNodeBase>& GetCurrentTaskNode() const;

    void                              AddTaskResultToPlan(const HTNTaskResult& inTaskResult);
    const std::vector<HTNTaskResult>& GetPlan() const;

    void            PushEnvironment();
    void            PushEnvironment(const HTNEnvironment& inEnvironment);
    void            PopEnvironment();
    HTNEnvironment& GetCurrentEnvironment();

private:
    // Tasks pending to be processed
    std::vector<HTNTaskInstance> mPendingTaskInstances;

    // Current task being processed
    std::shared_ptr<const HTNTaskNodeBase> mCurrentTaskNode;

    // Final plan of tasks
    std::vector<HTNTaskResult> mPlan;

    // State of variables and indices
    std::vector<HTNEnvironment> mEnvironments; // This should be called EnvironmentStack

    // TODO salvarez Path from current node to previous decomposition record node
    std::vector<std::shared_ptr<const HTNNodeBase>> mBacktrackStack;
};

class HTNDecompositionContext
{
public:
    HTNDecompositionContext(const HTNWorldState& inWorldState);

    const HTNWorldState* GetWorldState() const;

    void RecordDecomposition(HTNDecompositionRecord& inDecomposition);
    void RecordCurrentDecomposition();
    bool RestoreDecomposition();

    const HTNDecompositionRecord& GetCurrentDecomposition() const;
    HTNDecompositionRecord&       GetCurrentDecompositionMutable();

    void                                       SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory);
    const std::vector<HTNDecompositionRecord>& GetDecompositionHistory() const;

private:
    // TODO salvarez Maybe move the world state to the interpreter too
    const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at
                                                ///< all, this is why it is important this is a const pointer.

    HTNDecompositionRecord              mCurrentDecomposition;
    std::vector<HTNDecompositionRecord> mDecompositionHistory;
};

inline void HTNDecompositionRecord::PushPendingTaskNode(const std::shared_ptr<const HTNTaskNodeBase>& inPendingTaskNode)
{
    mPendingTaskInstances.emplace_back(inPendingTaskNode, GetCurrentEnvironment());
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

inline void HTNDecompositionRecord::SetCurrentTaskNode(const std::shared_ptr<const HTNTaskNodeBase>& inCurrentTaskNode)
{
    mCurrentTaskNode = inCurrentTaskNode;
}

inline const std::shared_ptr<const HTNTaskNodeBase>& HTNDecompositionRecord::GetCurrentTaskNode() const
{
    return mCurrentTaskNode;
}

inline void HTNDecompositionRecord::AddTaskResultToPlan(const HTNTaskResult& inTaskResult)
{
    mPlan.emplace_back(inTaskResult);
}

inline const std::vector<HTNTaskResult>& HTNDecompositionRecord::GetPlan() const
{
    return mPlan;
}

inline void HTNDecompositionRecord::PushEnvironment()
{
    mEnvironments.emplace_back();
}

inline void HTNDecompositionRecord::PushEnvironment(const HTNEnvironment& inEnvironment)
{
    mEnvironments.emplace_back(inEnvironment);
}

inline void HTNDecompositionRecord::PopEnvironment()
{
    mEnvironments.pop_back();
}

inline HTNEnvironment& HTNDecompositionRecord::GetCurrentEnvironment()
{
    return mEnvironments.back();
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

inline void HTNDecompositionContext::RecordCurrentDecomposition()
{
    RecordDecomposition(mCurrentDecomposition);
}

inline void HTNDecompositionContext::SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory)
{
    mDecompositionHistory = inDecompositionHistory;
}

inline const std::vector<HTNDecompositionRecord>& HTNDecompositionContext::GetDecompositionHistory() const
{
    return mDecompositionHistory;
}

inline const HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecomposition() const
{
    return mCurrentDecomposition;
}

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
    return mCurrentDecomposition;
}
