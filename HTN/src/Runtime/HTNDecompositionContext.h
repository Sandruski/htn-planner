#pragma once

#include "HTNAtom.h"

#include <memory>
#include <unordered_map>
#include <vector>

class HTNCondition;
class HTNConditionBase;
class HTNMethod;
class HTNTask;
class HTNWorldState;

class HTNDecompositionRecord
{
public:
	void SetCurrentTask(const std::shared_ptr<const HTNTask>& inTask);
	const std::shared_ptr<const HTNTask>& GetCurrentTask() const;

	void PushTaskToProcess(const std::shared_ptr<const HTNTask>& inTask);
	const std::shared_ptr<const HTNTask>& PopTaskToProcess();
	const std::vector<std::shared_ptr<const HTNTask>>& GetTasksToProcess() const;
	bool HasTasksToProcess() const;

	void AddTaskToPlan(const std::shared_ptr<const HTNTask>& inTask);
	const std::vector<std::shared_ptr<const HTNTask>>& GetPlan() const;

	unsigned int GetOrAddCurrentBranchIndex(const std::shared_ptr<const HTNMethod>& inMethod);
	unsigned int IncrementCurrentBranchIndex(const std::shared_ptr<const HTNMethod>& inMethod);

	unsigned int GetOrAddCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition);
	unsigned int IncrementCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition);

	unsigned int AddOrIncrementCurrentFactEntryIndex(const std::shared_ptr<const HTNCondition>& inCondition);

	void SetVariables(const std::shared_ptr<const HTNMethod>& inMethod, const std::unordered_map<std::string, HTNAtom>& inVariables);
	const std::unordered_map<std::string, HTNAtom>* FindVariables(const std::shared_ptr<const HTNMethod>& inMethod);
	HTNAtom& GetOrAddVariable(const std::shared_ptr<const HTNMethod>& inMethod, const std::string& inName);
	const HTNAtom* FindVariable(const std::shared_ptr<const HTNMethod>& inMethod, const std::string& inName) const;

private:
	std::shared_ptr<const HTNTask> mCurrentTask;
	std::vector<std::shared_ptr<const HTNTask>> mTasksToProcess;
	std::vector<std::shared_ptr<const HTNTask>> mPlan;
	std::unordered_map<std::shared_ptr<const HTNMethod>, unsigned int> mCurrentBranchIndex; // In a method there can be multiple branches
	std::unordered_map<std::shared_ptr<const HTNConditionBase>, unsigned int> mCurrentConditionIndex; // In a branch there can be one condition. In a compound condition there can be multiple single ones
	std::unordered_map<std::shared_ptr<const HTNCondition>, unsigned int> mCurrentFactEntryIndices; // A single condition can be checked against multiple facts ///< Index used to query the row in the database. It is initialized to -1 and incremented before querying the row in the database, so the first index used is 0
	std::unordered_map <std::shared_ptr<const HTNMethod>, std::unordered_map<std::string, HTNAtom>> mVariables; // In a method there can be multiple variables. Variables have the scope of a method
};

class HTNDecompositionContext
{
public:
	HTNDecompositionContext(const HTNWorldState& inWorldState);

	const HTNWorldState* GetWorldState() const;

    void SetCurrentMethod(const std::shared_ptr<const HTNMethod>& inMethod);
    const std::shared_ptr<const HTNMethod>& GetCurrentMethod() const;

	void RecordDecomposition(HTNDecompositionRecord& inDecomposition);
	void RecordCurrentDecomposition();
	bool RestoreDecomposition();

	HTNDecompositionRecord& GetCurrentDecompositionMutable();

private:
	const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at all, this is why it is important this is a const pointer.
	
	std::shared_ptr<const HTNMethod> mCurrentMethod;

	HTNDecompositionRecord mCurrentDecomposition;
	std::vector<HTNDecompositionRecord> mDecompositionHistory;
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
	mTasksToProcess.push_back(inTask);
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNDecompositionRecord::GetTasksToProcess() const
{
	return mTasksToProcess;
}

inline bool HTNDecompositionRecord::HasTasksToProcess() const
{
	return !mTasksToProcess.empty();
}

inline void HTNDecompositionRecord::AddTaskToPlan(const std::shared_ptr<const HTNTask>& inTask)
{
	mPlan.push_back(inTask);
}

inline const std::vector<std::shared_ptr<const HTNTask>>& HTNDecompositionRecord::GetPlan() const
{
	return mPlan;
}

inline unsigned int HTNDecompositionRecord::GetOrAddCurrentBranchIndex(const std::shared_ptr<const HTNMethod>& inMethod)
{
	return mCurrentBranchIndex[inMethod];
}

inline unsigned int HTNDecompositionRecord::IncrementCurrentBranchIndex(const std::shared_ptr<const HTNMethod>& inMethod)
{
	return ++mCurrentBranchIndex[inMethod];
}

inline unsigned int HTNDecompositionRecord::GetOrAddCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition)
{
	return mCurrentConditionIndex[inCondition];
}

inline unsigned int HTNDecompositionRecord::IncrementCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition)
{
	return ++mCurrentConditionIndex[inCondition];
}

inline unsigned int HTNDecompositionRecord::AddOrIncrementCurrentFactEntryIndex(const std::shared_ptr<const HTNCondition>& inCondition)
{
	return mCurrentFactEntryIndices.contains(inCondition) ? ++mCurrentFactEntryIndices[inCondition] : mCurrentFactEntryIndices[inCondition];
}

inline void HTNDecompositionRecord::SetVariables(const std::shared_ptr<const HTNMethod>& inMethod, const std::unordered_map<std::string, HTNAtom>& inVariables)
{
	mVariables[inMethod] = inVariables;
}

inline const std::unordered_map<std::string, HTNAtom>* HTNDecompositionRecord::FindVariables(const std::shared_ptr<const HTNMethod>& inMethod)
{
    const auto MethodIt = mVariables.find(inMethod);
    if (MethodIt == mVariables.end())
    {
        return nullptr;
    }

    return &MethodIt->second;
}

inline HTNAtom& HTNDecompositionRecord::GetOrAddVariable(const std::shared_ptr<const HTNMethod>& inMethod, const std::string& inName)
{
	return mVariables[inMethod][inName];
}

inline const HTNAtom* HTNDecompositionRecord::FindVariable(const std::shared_ptr<const HTNMethod>& inMethod, const std::string& inName) const
{
	const auto MethodIt = mVariables.find(inMethod);
    if (MethodIt == mVariables.end())
    {
        return nullptr;
    }

	const auto VariableIt = MethodIt->second.find(inName);
	if (VariableIt == MethodIt->second.end())
	{
		return nullptr;
	}

	return &VariableIt->second;
}

inline HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState)
	: mWorldState(&inWorldState)
{
}

inline const HTNWorldState* HTNDecompositionContext::GetWorldState() const
{
	return mWorldState;
}

inline void HTNDecompositionContext::SetCurrentMethod(const std::shared_ptr<const HTNMethod>& inMethod)
{
	mCurrentMethod = inMethod;
}

inline const std::shared_ptr<const HTNMethod>& HTNDecompositionContext::GetCurrentMethod() const
{
	return mCurrentMethod;
}

inline void HTNDecompositionContext::RecordDecomposition(HTNDecompositionRecord& inDecomposition)
{
	mDecompositionHistory.emplace_back(inDecomposition);
}

inline void HTNDecompositionContext::RecordCurrentDecomposition()
{
	RecordDecomposition(mCurrentDecomposition);
}

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
	return mCurrentDecomposition;
}
