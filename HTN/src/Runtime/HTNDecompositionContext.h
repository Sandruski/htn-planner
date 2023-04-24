#pragma once

#include <unordered_map>

class HTNCondition;
class HTNWorldState;

struct HTNDecompositionContext
{
public:
	HTNDecompositionContext(const HTNWorldState& inWorldState);

	const HTNWorldState* GetWorldState() const;

	int GetIndex(const HTNCondition& _Condition) const;

	int IncrementIndex(const HTNCondition& _Condition);

private:
	const HTNWorldState* mWorldState = nullptr; ///< Pointer to world state. All the queries will just not be able to modify the world state at all, this is why it is important this is a const pointer.
	std::unordered_map<const HTNCondition*, int> mIndices; ///< Index used to query the row in the database. It is initialized to -1 and incremented before querying the row in the database, so the first index used is 0
};

inline HTNDecompositionContext::HTNDecompositionContext(const HTNWorldState& inWorldState)
	: mWorldState(&inWorldState)
{
}

inline const HTNWorldState* HTNDecompositionContext::GetWorldState() const
{
	return mWorldState;
}

inline int HTNDecompositionContext::GetIndex(const HTNCondition& _Condition) const
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

inline int HTNDecompositionContext::IncrementIndex(const HTNCondition& _Condition)
{
	const bool ContainsCondition = mIndices.contains(&_Condition);

	int& Index = mIndices[&_Condition];

	if (ContainsCondition)
	{
		++Index;
	}

	return Index;
}
