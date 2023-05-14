#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

class HTNConditionBase;
class HTNTask;
class HTNWorldState;

class HTNDecompositionRecord
{
public:
	void PushTaskToProcess(const std::shared_ptr<const HTNTask>& inTask);
	const std::shared_ptr<const HTNTask>& PopTaskToProcess();
	const std::vector<std::shared_ptr<const HTNTask>>& GetTasksToProcess() const;
	bool HasTasksToProcess() const;

	void AddTaskToPlan(const std::shared_ptr<const HTNTask>& inTask);
	const std::vector<std::shared_ptr<const HTNTask>>& GetPlan() const;

	void IncrementNextBranchIndex();
	void ResetNextBranchIndex();
	unsigned int GetNextBranchIndex() const;

private:
	std::vector<std::shared_ptr<const HTNTask>> mTasksToProcess;
	std::vector<std::shared_ptr<const HTNTask>> mPlan;
	unsigned int mNextBranchIndex = 0;
};

class HTNDecompositionContext
{
public:
	HTNDecompositionContext(const HTNWorldState& inWorldState);

	const HTNWorldState* GetWorldState() const;

	int GetIndex(const HTNConditionBase& _Condition) const;

	int IncrementIndex(const HTNConditionBase& _Condition);

	void RecordDecomposition();
	void RestoreDecomposition();

	HTNDecompositionRecord& GetCurrentDecompositionMutable();

private:
	const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at all, this is why it is important this is a const pointer.
	std::unordered_map<const HTNConditionBase*, int> mIndices; ///< Index used to query the row in the database. It is initialized to -1 and incremented before querying the row in the database, so the first index used is 0
	HTNDecompositionRecord mCurrentDecomposition;
	std::vector<HTNDecompositionRecord> mDecompositionHistory;
};

inline void HTNDecompositionRecord::PushTaskToProcess(const std::shared_ptr<const HTNTask>& inTask)
{
	mTasksToProcess.push_back(inTask);
}

inline const std::shared_ptr<const HTNTask>& HTNDecompositionRecord::PopTaskToProcess()
{
	const std::shared_ptr<const HTNTask>& Task = mTasksToProcess.back();
	mTasksToProcess.pop_back();
	return Task;
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

inline void HTNDecompositionRecord::IncrementNextBranchIndex()
{
	++mNextBranchIndex;
}

inline void HTNDecompositionRecord::ResetNextBranchIndex()
{
	mNextBranchIndex = 0;
}

inline unsigned int HTNDecompositionRecord::GetNextBranchIndex() const
{
	return mNextBranchIndex;
}

inline HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState)
	: mWorldState(&inWorldState)
{
}

inline const HTNWorldState* HTNDecompositionContext::GetWorldState() const
{
	return mWorldState;
}

inline int HTNDecompositionContext::GetIndex(const HTNConditionBase& _Condition) const
{
	const auto It = mIndices.find(&_Condition);
	if (It == mIndices.end())
	{
		// Index not found
		return -1;
	}

	const int Index = It->second;
	return Index;
}

inline int HTNDecompositionContext::IncrementIndex(const HTNConditionBase& _Condition)
{
	const bool ContainsCondition = mIndices.contains(&_Condition);

	int& Index = mIndices[&_Condition];

	if (ContainsCondition)
	{
		++Index;
	}

	return Index;
}

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
	return mCurrentDecomposition;
}

inline void HTNDecompositionContext::RecordDecomposition()
{
	mDecompositionHistory.emplace_back(mCurrentDecomposition);
}

inline void HTNDecompositionContext::RestoreDecomposition()
{
	if (mDecompositionHistory.empty())
	{
		return;
	}

	mCurrentDecomposition = mDecompositionHistory.back();
	mDecompositionHistory.pop_back();
}
