#include "Runtime/HTNDecompositionContext.h"

const std::shared_ptr<const HTNTask>& HTNDecompositionRecord::PopTaskToProcess()
{
	const std::shared_ptr<const HTNTask>& Task = mTasksToProcess.back();
	mTasksToProcess.pop_back();
	return Task;
}

void HTNDecompositionContext::RestoreDecomposition()
{
	if (mDecompositionHistory.empty())
	{
		return;
	}

	mCurrentDecomposition = mDecompositionHistory.back();
	mDecompositionHistory.pop_back();
}
