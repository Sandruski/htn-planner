#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNWorldState.h"

#include <array>
#include <string>
#include <type_traits>

class HTNAtom;
struct HTNDecompositionContext;

class HTNCondition : public HTNNodeBase
{
public:
	HTNCondition();
	~HTNCondition();

	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	// Check if the condition is true.
	virtual bool Check(HTNDecompositionContext& ioContext) const = 0;

private:
	// TODO salvarez
};

template <int NumArgs>
class HTNConditionWorldStateQuery final : public HTNCondition
{
public:
	bool Check(HTNDecompositionContext& ioContext) const final;

	void SetKey(const std::string& inKey);
	void SetArgument(const int inIndex, HTNAtom* inAtom);

private:
	template <std::size_t... Is>
	bool Check(const HTNWorldState& inWorldState, const int inIndex, std::index_sequence<Is...>) const;

	std::string mKey;
	std::array<HTNAtom*, NumArgs> mArgs;
};

template <int NumArgs>
inline void HTNConditionWorldStateQuery<NumArgs>::SetKey(const std::string& inKey)
{
	mKey = inKey;
}

template <int NumArgs>
inline void HTNConditionWorldStateQuery<NumArgs>::SetArgument(const int inIndex, HTNAtom* inAtom)
{
	if (inIndex < 0 || inIndex >= NumArgs)
	{
		return;
	}

	mArgs[inIndex] = inAtom;
}

template <int NumArgs>
inline bool HTNConditionWorldStateQuery<NumArgs>::Check(HTNDecompositionContext& ioContext) const
{
	const HTNWorldState* WorldState = ioContext.GetWorldState();
	if (!WorldState)
	{
		return false;
	}

	const int Index = ioContext.IncrementIndex(*this);

	return Check(*WorldState, Index, std::make_index_sequence<NumArgs>{});
}

template <int NumArgs>
template <std::size_t... I>
inline bool HTNConditionWorldStateQuery<NumArgs>::Check(const HTNWorldState& inWorldState, const int inIndex, std::index_sequence<I...>) const
{
	return inWorldState.CheckIndex(mKey.c_str(), inIndex, *mArgs[I]...);
}
