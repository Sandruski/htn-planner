#pragma once

#include "Domain/Interpreter/HTNEnvironment.h"
#include "Domain/Interpreter/HTNNodePath.h"
#include "Domain/Interpreter/HTNTaskInstance.h"
#include "Domain/Interpreter/HTNTaskResult.h"
#include "Domain/Nodes/HTNNodeVisitorContextBase.h"

#include <memory>
#include <string>
#include <vector>

#ifdef HTN_DEBUG
#include "Domain/Interpreter/HTNDecompositionSnapshotDebug.h"
#endif

class HTNDomainNode;
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

class HTNDecompositionContext final : public HTNNodeVisitorContextBase
{
public:
    HTNDecompositionContext() = default;
    explicit HTNDecompositionContext(const HTNWorldState& inWorldState);

    const HTNWorldState* GetWorldState() const;

    void                                        SetDomainNode(const std::shared_ptr<const HTNDomainNode>& inDomainNode);
    const std::shared_ptr<const HTNDomainNode>& GetDomainNode() const;

    void               SetEntryPointID(const std::string& inEntryPointID);
    const std::string& GetEntryPointID() const;

    void RecordDecomposition(HTNDecompositionRecord& inDecomposition);
    bool RestoreDecomposition();

    const HTNDecompositionRecord& GetCurrentDecomposition() const;
    HTNDecompositionRecord&       GetCurrentDecompositionMutable();

    void                                       SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory);
    const std::vector<HTNDecompositionRecord>& GetDecompositionHistory() const;

    void               SetCurrentNodePath(const HTNNodePath& inCurrentNodePath);
    const HTNNodePath& GetCurrentNodePath() const;
    HTNNodePath&       GetCurrentNodePathMutable();

    void               SetCurrentVariableScopeNodePath(const HTNNodePath& inCurrentVariableScopeNodePath);
    const HTNNodePath& GetCurrentVariableScopeNodePath() const;
    HTNNodePath&       GetCurrentVariableScopeNodePathMutable();

private:
    //----------------------------------------------------------------------//
    // Input
    //----------------------------------------------------------------------//
    const HTNWorldState*                 mWorldState = nullptr;
    std::shared_ptr<const HTNDomainNode> mDomainNode;
    std::string                          mEntryPointID;

    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
    // Current decomposition
    HTNDecompositionRecord mCurrentDecomposition;

    //----------------------------------------------------------------------//
    // Internal
    //----------------------------------------------------------------------//
    // Record of previous decompositions
    std::vector<HTNDecompositionRecord> mDecompositionHistory;

    // Path from the root node to the current node being processed
    HTNNodePath mCurrentNodePath;

    // Path from the root node to the current node determining the scope of the variables
    HTNNodePath mCurrentVariableScopeNodePath;

#ifdef HTN_DEBUG
public:
    void RecordNodeSnapshot(const std::string& inNodePath, const bool inResult, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);
    void RecordNodeSnapshot(const std::string& inNodePath, const HTNNodeStep inNodeStep, const bool inIsChoicePoint);

    const HTNDecompositionSnapshotDebug& GetDecompositionSnapshot() const;

private:
    //----------------------------------------------------------------------//
    // Output
    //----------------------------------------------------------------------//
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

inline const HTNWorldState* HTNDecompositionContext::GetWorldState() const
{
    return mWorldState;
}

inline void HTNDecompositionContext::SetDomainNode(const std::shared_ptr<const HTNDomainNode>& inDomainNode)
{
    mDomainNode = inDomainNode;
}

inline const std::shared_ptr<const HTNDomainNode>& HTNDecompositionContext::GetDomainNode() const
{
    return mDomainNode;
}

inline void HTNDecompositionContext::SetEntryPointID(const std::string& inEntryPointID)
{
    mEntryPointID = inEntryPointID;
}

inline const std::string& HTNDecompositionContext::GetEntryPointID() const
{
    return mEntryPointID;
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

inline void HTNDecompositionContext::SetCurrentNodePath(const HTNNodePath& inCurrentNodePath)
{
    mCurrentNodePath = inCurrentNodePath;
}

inline const HTNNodePath& HTNDecompositionContext::GetCurrentNodePath() const
{
    return mCurrentNodePath;
}

inline HTNNodePath& HTNDecompositionContext::GetCurrentNodePathMutable()
{
    return mCurrentNodePath;
}

inline void HTNDecompositionContext::SetCurrentVariableScopeNodePath(const HTNNodePath& inCurrentVariableScopeNodePath)
{
    mCurrentVariableScopeNodePath = inCurrentVariableScopeNodePath;
}

inline const HTNNodePath& HTNDecompositionContext::GetCurrentVariableScopeNodePath() const
{
    return mCurrentVariableScopeNodePath;
}

inline HTNNodePath& HTNDecompositionContext::GetCurrentVariableScopeNodePathMutable()
{
    return mCurrentVariableScopeNodePath;
}

#ifdef HTN_DEBUG
inline const HTNDecompositionSnapshotDebug& HTNDecompositionContext::GetDecompositionSnapshot() const
{
    return mDecompositionSnapshot;
}
#endif
