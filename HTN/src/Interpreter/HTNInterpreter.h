#pragma once

#include <memory>
#include <string>
#include <vector>

class HTNDomain;
class HTNTask;
class HTNWorldState;
class HTNTask;

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
	//std::shared_ptr<const HTNTask> CurrentTask;
	std::vector<std::shared_ptr<const HTNTask>> mPlan;
	unsigned int mNextBranchIndex = 0;
};

// Traverses an abstract syntax tree
class HTNInterpreter
{
public:
	explicit HTNInterpreter(const std::shared_ptr<const HTNDomain>& inDomain, const std::string& inRootCompoundTaskName);
	~HTNInterpreter();

	std::vector<std::shared_ptr<const HTNTask>> Interpret(const HTNWorldState& inWorldState);

private:
	void RecordDecomposition();
	void RestoreDecomposition();

	std::shared_ptr<const HTNDomain> mDomain;
	std::string mRootCompoundTaskName;

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
