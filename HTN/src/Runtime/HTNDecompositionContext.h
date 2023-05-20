#pragma once

#include "HTNAtom.h"

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

	int GetIndex(const std::shared_ptr<const HTNConditionBase>& inCondition) const;
	int IncrementIndex(const std::shared_ptr<const HTNConditionBase>& inCondition);

	HTNAtom& AddOrGetVariable(const std::string& inName);
	const HTNAtom* FindVariable(const std::string& inName) const;

	void RecordDecomposition();
	void RestoreDecomposition();

	HTNDecompositionRecord& GetCurrentDecompositionMutable();

private:
	const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at all, this is why it is important this is a const pointer.
	std::unordered_map<std::shared_ptr<const HTNConditionBase>, int> mIndices; ///< Index used to query the row in the database. It is initialized to -1 and incremented before querying the row in the database, so the first index used is 0
	std::unordered_map<std::string, HTNAtom> mVariables;
	HTNDecompositionRecord mCurrentDecomposition;
	std::vector<HTNDecompositionRecord> mDecompositionHistory;
};

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

inline HTNAtom& HTNDecompositionContext::AddOrGetVariable(const std::string& inName)
{
	return mVariables[inName];
}

inline const HTNAtom* HTNDecompositionContext::FindVariable(const std::string& inName) const
{
	const auto It = mVariables.find(inName);
	if (It == mVariables.end())
	{
		return nullptr;
	}

	return &It->second;
}

inline void HTNDecompositionContext::RecordDecomposition()
{
	mDecompositionHistory.emplace_back(mCurrentDecomposition);
}

inline HTNDecompositionRecord& HTNDecompositionContext::GetCurrentDecompositionMutable()
{
	return mCurrentDecomposition;
}
