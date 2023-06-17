#include "Runtime/HTNDecompositionContext.h"

const std::shared_ptr<const HTNTask>& HTNDecompositionRecord::PopTaskToProcess()
{
	const std::shared_ptr<const HTNTask>& Task = mTasksToProcess.back();
	mTasksToProcess.pop_back();
	return Task;
}

bool HTNDecompositionContext::RestoreDecomposition()
{
	if (mDecompositionHistory.empty())
	{
		mCurrentDecomposition = HTNDecompositionRecord();
		return false;
	}

	mCurrentDecomposition = mDecompositionHistory.back();
	mDecompositionHistory.pop_back();

	return true;
}
