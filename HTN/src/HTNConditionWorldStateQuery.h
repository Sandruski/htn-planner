#pragma once

#include "HTNCondition.h"
#include "HTNDecompositionContext.h"

#include <array>
#include <type_traits>

class HTNAtom;

template <int NumArgs>
class HTNConditionWorldStateQuery final : public HTNCondition
{
public:
	// Check if the condition is true.
	bool Check(HTNDecompositionContext& ioContext) final;

	void SetKey(const char* inKey)
	{ 
		mKey = inKey; 
	}

	void SetArgument(int inIndex, HTNAtom* inAtom)
	{
		if (inIndex < 0 || inIndex >= NumArgs)
		{
			return;
		}

		mArgs[inIndex] = inAtom;
	}

private:
	template <std::size_t... Is>
	bool Check(const HTNWorldState& inWorldState, std::index_sequence<Is...>);

	const char* mKey = nullptr;
	std::array<HTNAtom*, NumArgs> mArgs;
};

template <int NumArgs>
inline bool HTNConditionWorldStateQuery<NumArgs>::Check(HTNDecompositionContext& ioContext)
{
	const HTNWorldState* WorldState = ioContext.GetWorldState();
	if (!WorldState)
	{
		return false;
	}

	return Check(*WorldState, std::make_index_sequence<NumArgs>{});
}

template <int NumArgs>
template <std::size_t... I>
inline bool HTNConditionWorldStateQuery<NumArgs>::Check(const HTNWorldState& inWorldState, std::index_sequence<I...>)
{
	int Index = 0; // TODO salvarez Store the index in the context
	return inWorldState.CheckIndex(mKey, Index, *mArgs[I]...);
}
