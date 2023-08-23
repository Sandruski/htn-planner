#pragma once

#include "HTNAtom.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class HTNDomain;
class HTNMethod;
class HTNTask;
class HTNWorldState;
class HTNNodeBase;

class HTNDecompositionRecord
{
public:
    void                                  SetCurrentTask(const std::shared_ptr<const HTNTask>& inTask);
    const std::shared_ptr<const HTNTask>& GetCurrentTask() const;

    void                                  PushTaskToProcess(const std::shared_ptr<const HTNTask>& inTask);
    const std::shared_ptr<const HTNTask>& PopTaskToProcess();
    bool                                  HasTasksToProcess() const;

    // Returns an existing index or adds a new index
    unsigned int GetOrAddIndex(const std::string& inScopeID);

    // Increments an existing index or adds a new index and increments it
    unsigned int IncrementIndex(const std::string& inScopeID);

    // Adds a new index or increments an existing index
    unsigned int AddOrIncrementIndex(const std::string& inScopeID);

    void SetVariables(const std::string& inScopeID, const std::unordered_map<std::string, HTNAtom>& inVariables);
    const std::unordered_map<std::string, HTNAtom>* FindVariables(const std::string& inScopeID) const;
    const std::unordered_map<std::string, HTNAtom>& GetVariables(const std::string& inScopeID) const;
    bool                                            HasVariables(const std::string& inScopeID) const;
    HTNAtom&                                        GetOrAddVariable(const std::string& inScopeID, const std::string& inVariableName);
    const HTNAtom*                                  FindVariable(const std::string& inScopeID, const std::string& inVariableName) const;

    void                                               AddTaskToPlan(const std::shared_ptr<const HTNTask>& inTask);
    const std::vector<std::shared_ptr<const HTNTask>>& GetPlan() const;

private:
    // Current task being processed
    std::shared_ptr<const HTNTask> mCurrentTask;

    // Tasks pending to be processed
    // TODO salvarez RELATED Include scope here somehow
    std::vector<std::shared_ptr<const HTNTask>> mTasksToProcess;

    // Scope ID to index
    // - Method scope ID to branch index
    // - HTNConditionAnd, HTNConditionOr, and HTNConditionAlt scope ID to sub-condition index
    // - HTNCondition scope ID to fact entry index
    std::unordered_map<std::string, unsigned int> mIndices;

    // Scope ID to variable (name and value)
    std::unordered_map<std::string, std::unordered_map<std::string, HTNAtom>> mVariables;

    // Final plan of tasks
    // TODO salvarez RELATED Include scope/variables here somehow
    std::vector<std::shared_ptr<const HTNTask>> mPlan;
};

class HTNDecompositionContext
{
public:
    HTNDecompositionContext(const HTNWorldState& inWorldState);

    const HTNWorldState* GetWorldState() const;

    void                                      PushNodeToCurrentScope(const std::shared_ptr<const HTNNodeBase>& inNode);
    const std::shared_ptr<const HTNNodeBase>& PopNodeFromCurrentScope();
    const std::shared_ptr<const HTNDomain>    GetCurrentDomain() const;
    const std::shared_ptr<const HTNMethod>    GetCurrentMethod() const;
    std::string                               MakeCurrentScopeID() const;

    void RecordDecomposition(HTNDecompositionRecord& inDecomposition);
    void RecordCurrentDecomposition();
    bool RestoreDecomposition();

    void                                       SetDecompositionHistory(const std::vector<HTNDecompositionRecord>& inDecompositionHistory);
    const std::vector<HTNDecompositionRecord>& GetDecompositionHistory() const;

    const HTNDecompositionRecord& GetCurrentDecomposition() const;
    HTNDecompositionRecord&       GetCurrentDecompositionMutable();

private:
    const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at
                                                ///< all, this is why it is important this is a const pointer.

    std::vector<std::shared_ptr<const HTNNodeBase>> mCurrentScope;

    HTNDecompositionRecord              mCurrentDecomposition;
    std::vector<HTNDecompositionRecord> mDecompositionHistory;
};

/**
 * Helper class that uses RAII (Resource Acquisition Is Initialization) to automatically push/pop a node to/from the current scope of a decomposition
 * context
 */
class HTNDecompositionScope
{
public:
    HTNDecompositionScope(HTNDecompositionContext& inDecompositionContext, const std::shared_ptr<const HTNNodeBase>& inNode);
    ~HTNDecompositionScope();

private:
    HTNDecompositionContext&           mDecompositionContext;
    std::shared_ptr<const HTNNodeBase> mNode;
};

inline void HTNDecompositionRecord::SetCurrentTask(const std::shared_ptr<const HTNTask>& inTask)
{
    mCurrentTask = inTask;
}

inline const std::shared_ptr<const HTNTask>& HTNDecompositionRecord::GetCurrentTask() const
{
    return mCurrentTask;
}

inline void HTNDecompositionRecord::PushTaskToProcess(const std::shared_ptr<const HTNTask>& inTask)
{
    mTasksToProcess.emplace_back(inTask);
}

inline const std::shared_ptr<const HTNTask>& HTNDecompositionRecord::PopTaskToProcess()
{
    const std::shared_ptr<const HTNTask>& Task = mTasksToProcess.back();
    mTasksToProcess.pop_back();
    return Task;
}

inline bool HTNDecompositionRecord::HasTasksToProcess() const
{
    return !mTasksToProcess.empty();
}

inline unsigned int HTNDecompositionRecord::GetOrAddIndex(const std::string& inScopeID)
{
    return mIndices[inScopeID];
}

inline unsigned int HTNDecompositionRecord::IncrementIndex(const std::string& inScopeID)
{
    return ++mIndices[inScopeID];
}

inline unsigned int HTNDecompositionRecord::AddOrIncrementIndex(const std::string& inScopeID)
{
    auto ScopeIt = mIndices.find(inScopeID);
    if (ScopeIt == mIndices.end())
    {
        return mIndices[inScopeID];
    }

    return ++ScopeIt->second;
}

inline void HTNDecompositionRecord::SetVariables(const std::string& inScopeID, const std::unordered_map<std::string, HTNAtom>& inVariables)
{
    mVariables[inScopeID] = inVariables;
}

inline const std::unordered_map<std::string, HTNAtom>* HTNDecompositionRecord::FindVariables(const std::string& inScopeID) const
{
    const auto ScopeIt = mVariables.find(inScopeID);
    if (ScopeIt == mVariables.end())
    {
        return nullptr;
    }

    return &ScopeIt->second;
}

inline const std::unordered_map<std::string, HTNAtom>& HTNDecompositionRecord::GetVariables(const std::string& inScopeID) const
{
    const auto ScopeIt = mVariables.find(inScopeID);
    return ScopeIt->second;
}

inline bool HTNDecompositionRecord::HasVariables(const std::string& inScopeID) const
{
    const auto ScopeIt = mVariables.find(inScopeID);
    return (ScopeIt != mVariables.end());
}

inline HTNAtom& HTNDecompositionRecord::GetOrAddVariable(const std::string& inScopeID, const std::string& inVariableName)
{
    return mVariables[inScopeID][inVariableName];
}

inline const HTNAtom* HTNDecompositionRecord::FindVariable(const std::string& inScopeID, const std::string& inVariableName) const
{
    const auto ScopeIt = mVariables.find(inScopeID);
    if (ScopeIt == mVariables.end())
    {
        return nullptr;
    }

    const auto VariableIt = ScopeIt->second.find(inVariableName);
    if (VariableIt == ScopeIt->second.end())
    {
        return nullptr;
    }

    return &VariableIt->second;
}

inline void HTNDecompositionRecord::AddTaskToPlan(const std::shared_ptr<const HTNTask>& inTask)
{
    mPlan.emplace_back(inTask);
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNDecompositionRecord::GetPlan() const
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

inline void HTNDecompositionContext::PushNodeToCurrentScope(const std::shared_ptr<const HTNNodeBase>& inNode)
{
    mCurrentScope.emplace_back(inNode);
}

inline const std::shared_ptr<const HTNNodeBase>& HTNDecompositionContext::PopNodeFromCurrentScope()
{
    const std::shared_ptr<const HTNNodeBase>& Node = mCurrentScope.back();
    mCurrentScope.pop_back();
    return Node;
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

inline HTNDecompositionScope::HTNDecompositionScope(HTNDecompositionContext& inDecompositionContext, const std::shared_ptr<const HTNNodeBase>& inNode)
    : mDecompositionContext(inDecompositionContext), mNode(inNode)
{
    mDecompositionContext.PushNodeToCurrentScope(mNode);
}

inline HTNDecompositionScope::~HTNDecompositionScope()
{
    const std::shared_ptr<const HTNNodeBase>& Node = mDecompositionContext.PopNodeFromCurrentScope();
    assert(mNode == Node);
}
