#pragma once

#include "Interpreter/AST/HTNNodeVisitorBase.h"

#include <memory>
#include <string>
#include <vector>

class HTNDomain;
class HTNPrimitiveTask;
class HTNWorldState;
class HTNTask;

class HTNDecompositionRecord
{
public:
	HTNDecompositionRecord() = default;
	explicit HTNDecompositionRecord(const std::vector<std::shared_ptr<const HTNTask>>& inTasksToProcess, const HTNPlan& inPlan, const unsigned int inNextBranchIndex);

	void PushTaskToProcess(const std::shared_ptr<const HTNTask>& inTask);
	const std::shared_ptr<const HTNTask>& PopTaskToProcess();
	const std::vector<std::shared_ptr<const HTNTask>>& GetTasksToProcess() const;
	bool HasTasksToProcess() const;

	void AddTaskToPlan(const std::shared_ptr<const HTNTask>& inTask);
	const std::vector<std::shared_ptr<const HTNTask>>& GetPlan() const;

private:
	std::vector<std::shared_ptr<const HTNTask>> mTasksToProcess;
	//std::shared_ptr<const HTNTask> CurrentTask;
	std::vector<std::shared_ptr<const HTNTask>> mPlan;
	unsigned int mNextBranchIndex = 0;
};

// Traverses an abstract syntax tree
class HTNInterpreter final : public HTNNodeVisitorBase
{
public:
	explicit HTNInterpreter(const std::shared_ptr<const HTNDomain>& inDomain, const std::string& inRootCompoundTaskName);
	~HTNInterpreter();

	std::vector<std::shared_ptr<const HTNPrimitiveTask>> Interpret(const HTNWorldState& inWorldState);

private:
	void RecordDecomposition();
	void RestoreDecomposition();

	std::shared_ptr<const HTNDomain> mDomain;
	std::string mRootCompoundTaskName;

	HTNDecompositionRecord mCurrentDecomposition;
	std::vector<const HTNDecompositionRecord> mDecompositionHistory;
};

inline HTNDecompositionRecord::HTNDecompositionRecord(const std::vector<std::shared_ptr<const HTNTask>>& inTasksToProcess, const HTNPlan& inPlan, const unsigned int inNextBranchIndex)
	: mTasksToProcess(inTasksToProcess), mPlan(inPlan), mNextBranchIndex(inNextBranchIndex)
{
}

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
