#pragma once

#include "HTNAtom.h"
#include "Interpreter/HTNEnvironment.h"
#include "Interpreter/HTNTaskInstance.h"

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
    void                   SetCurrentTaskInstance(const HTNTaskInstance& inCurrentTaskInstance);
    const HTNTaskInstance& GetCurrentTaskInstance() const;

    void            PushEnvironment();
    void            PopEnvironment();
    HTNEnvironment& GetCurrentEnvironment();
    HTNEnvironment& GetPreviousEnvironment();

    void            PushTaskInstanceToProcess(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode);
    HTNTaskInstance PopTaskInstanceToProcess();
    bool            HasTaskInstancesToProcess() const;

    void                                AddTaskToPlan(const HTNTaskInstance& inTaskInstance);
    const std::vector<HTNTaskInstance>& GetPlan() const;

private:
    // TODO salvarez Make the current compound task node a HTNTaskInstance and add the std::vector<HTNEnvironment> there?
    // Current compound task being processed
    HTNTaskInstance mCurrentTaskInstance;

    std::vector<HTNEnvironment> mEnvironments;

    // Tasks pending to be processed
    std::vector<HTNTaskInstance> mTaskInstancesToProcess;

    // Final plan of tasks
    std::vector<HTNTaskInstance> mPlan;
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

inline void HTNDecompositionRecord::SetCurrentTaskInstance(const HTNTaskInstance& inCurrentTaskInstance)
{
    mCurrentTaskInstance = inCurrentTaskInstance;
}

inline const HTNTaskInstance& HTNDecompositionRecord::GetCurrentTaskInstance() const
{
    return mCurrentTaskInstance;
}

inline void HTNDecompositionRecord::PushEnvironment()
{
    mEnvironments.emplace_back();
}

inline void HTNDecompositionRecord::PopEnvironment()
{
    mEnvironments.pop_back();
}

inline HTNEnvironment& HTNDecompositionRecord::GetCurrentEnvironment()
{
    return mEnvironments.back();
}

inline HTNEnvironment& HTNDecompositionRecord::GetPreviousEnvironment()
{
    return mEnvironments[mEnvironments.size() - 2];
}

inline void HTNDecompositionRecord::PushTaskInstanceToProcess(const std::shared_ptr<const HTNTaskNodeBase>& inTaskNode)
{
    mTaskInstancesToProcess.emplace_back(inTaskNode, GetCurrentEnvironment());
}

inline HTNTaskInstance HTNDecompositionRecord::PopTaskInstanceToProcess()
{
    const HTNTaskInstance TaskInstance = mTaskInstancesToProcess.back();
    mTaskInstancesToProcess.pop_back();
    return TaskInstance;
}

inline bool HTNDecompositionRecord::HasTaskInstancesToProcess() const
{
    return !mTaskInstancesToProcess.empty();
}

inline void HTNDecompositionRecord::AddTaskToPlan(const HTNTaskInstance& inTaskInstance)
{
    mPlan.emplace_back(inTaskInstance);
}

inline const std::vector<HTNTaskInstance>& HTNDecompositionRecord::GetPlan() const
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
