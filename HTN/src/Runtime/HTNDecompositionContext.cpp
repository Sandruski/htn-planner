#include "Runtime/HTNDecompositionContext.h"

const std::shared_ptr<const HTNTask>& HTNDecompositionRecord::PopTaskToProcess()
{
	const std::shared_ptr<const HTNTask>& Task = mTasksToProcess.back();
	mTasksToProcess.pop_back();
	return Task;
}

int HTNDecompositionContext::GetIndex(const HTNConditionBase& inCondition) const
{
	const auto It = mIndices.find(&inCondition);
	if (It == mIndices.end())
	{
		// Index not found
		return -1;
	}

	const int Index = It->second;
	return Index;
}

int HTNDecompositionContext::IncrementIndex(const HTNConditionBase& inCondition)
{
	const bool ContainsCondition = mIndices.contains(&inCondition);

	int& Index = mIndices[&inCondition];

	if (ContainsCondition)
	{
		++Index;
	}

	return Index;
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
