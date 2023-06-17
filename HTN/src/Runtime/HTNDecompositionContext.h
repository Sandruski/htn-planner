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
	unsigned int ResetCurrentBranchIndex(const std::shared_ptr<const HTNMethod>& inMethod);

	unsigned int GetOrAddCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition);
	unsigned int IncrementCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition);
	unsigned int ResetCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition);

	unsigned int AddOrIncrementCurrentFactEntryIndex(const std::shared_ptr<const HTNCondition>& inCondition);
	unsigned int ResetCurrentFactEntryIndex(const std::shared_ptr<const HTNCondition>& inCondition);

	void SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables);
	const std::unordered_map<std::string, HTNAtom>& GetVariables() const;
	HTNAtom& GetOrAddVariable(const std::string& inName);
	const HTNAtom* FindVariable(const std::string& inName) const;

private:
	std::shared_ptr<const HTNTask> mCurrentTask;
	std::vector<std::shared_ptr<const HTNTask>> mTasksToProcess;
	std::vector<std::shared_ptr<const HTNTask>> mPlan;
	std::unordered_map<std::shared_ptr<const HTNMethod>, unsigned int> mCurrentBranchIndex; // In a method there can be multiple branches
	std::unordered_map<std::shared_ptr<const HTNConditionBase>, unsigned int> mCurrentConditionIndex; // In a branch there can be one condition. In a compound condition there can be multiple single ones
	std::unordered_map<std::shared_ptr<const HTNCondition>, unsigned int> mCurrentFactEntryIndices; // A single condition can be checked against multiple facts. ///< Index used to query the row in the database. It is initialized to -1 and incremented before querying the row in the database, so the first index used is 0
	std::unordered_map<std::string, HTNAtom> mVariables;
};

class HTNDecompositionContext
{
public:
	HTNDecompositionContext(const HTNWorldState& inWorldState);

	const HTNWorldState* GetWorldState() const;

	void RecordDecomposition(HTNDecompositionRecord& inDecomposition);
	void RecordCurrentDecomposition();
	void RestoreDecomposition();

	HTNDecompositionRecord& GetCurrentDecompositionMutable();

private:
	const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at all, this is why it is important this is a const pointer.
	
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

inline unsigned int HTNDecompositionRecord::ResetCurrentBranchIndex(const std::shared_ptr<const HTNMethod>& inMethod)
{
	return mCurrentBranchIndex[inMethod] = 0;
}

inline unsigned int HTNDecompositionRecord::GetOrAddCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition)
{
	return mCurrentConditionIndex[inCondition];
}

inline unsigned int HTNDecompositionRecord::IncrementCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition)
{
	return ++mCurrentConditionIndex[inCondition];
}

inline unsigned int HTNDecompositionRecord::ResetCurrentConditionIndex(const std::shared_ptr<const HTNConditionBase>& inCondition)
{
	return mCurrentConditionIndex[inCondition] = 0;
}

inline unsigned int HTNDecompositionRecord::AddOrIncrementCurrentFactEntryIndex(const std::shared_ptr<const HTNCondition>& inCondition)
{
	return mCurrentFactEntryIndices.contains(inCondition) ? ++mCurrentFactEntryIndices[inCondition] : mCurrentFactEntryIndices[inCondition];
}

inline unsigned int HTNDecompositionRecord::ResetCurrentFactEntryIndex(const std::shared_ptr<const HTNCondition>& inCondition)
{
	return mCurrentFactEntryIndices[inCondition] = 0;
}

inline void HTNDecompositionRecord::SetVariables(const std::unordered_map<std::string, HTNAtom>& inVariables)
{
	mVariables = inVariables;
}

inline const std::unordered_map<std::string, HTNAtom>& HTNDecompositionRecord::GetVariables() const
{
	return mVariables;
}

inline HTNAtom& HTNDecompositionRecord::GetOrAddVariable(const std::string& inName)
{
	return mVariables[inName];
}

inline const HTNAtom* HTNDecompositionRecord::FindVariable(const std::string& inName) const
{
	const auto It = mVariables.find(inName);
	if (It == mVariables.end())
	{
		return nullptr;
	}

	return &It->second;
}

inline HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState)
	: mWorldState(&inWorldState)
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

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
	return mCurrentDecomposition;
}
