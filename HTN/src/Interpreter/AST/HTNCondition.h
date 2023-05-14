#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNWorldState.h"

#include <array>
#include <string>
#include <type_traits>

class HTNAtom;
class HTNDecompositionContext;

class HTNConditionBase : public HTNNodeBase
{
public:
	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;
	std::string ToString() const final;

	// Check if the condition is true.
	virtual bool Check(HTNDecompositionContext& ioContext) const = 0;
};

class HTNConditionWorldStateQuery final : public HTNConditionBase
{
public:
	bool Check(HTNDecompositionContext& ioContext) const final;

	void SetKey(const std::string& inKey);
	void SetArgument(HTNAtom* inAtom);

private:
	std::string mKey;
	std::vector<HTNAtom*> mArgs;
};

class HTNConditionAnd final : public HTNConditionBase
{
public:
	explicit HTNConditionAnd(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions);

	bool Check(HTNDecompositionContext& ioContext) const final;

private:
	std::vector<std::shared_ptr<const HTNConditionBase>> mConditions;
};

class HTNConditionOr final : public HTNConditionBase
{
public:
	explicit HTNConditionOr(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions);

	bool Check(HTNDecompositionContext& ioContext) const final;

private:
	std::vector<std::shared_ptr<const HTNConditionBase>> mConditions;
};

class HTNConditionNot final : public HTNConditionBase
{
public:
	explicit HTNConditionNot(const std::shared_ptr<const HTNConditionBase>& inCondition);

	bool Check(HTNDecompositionContext& ioContext) const final;

private:
	std::shared_ptr<const HTNConditionBase> mCondition;
};

inline void HTNConditionWorldStateQuery::SetKey(const std::string& inKey)
{
	mKey = inKey;
}

inline void HTNConditionWorldStateQuery::SetArgument(HTNAtom* inAtom)
{
	mArgs.emplace_back(inAtom);
}

inline bool HTNConditionWorldStateQuery::Check(HTNDecompositionContext& ioContext) const
{
	const HTNWorldState* WorldState = ioContext.GetWorldState();
	if (!WorldState)
	{
		return false;
	}

	const int Index = ioContext.IncrementIndex(*this);
	return WorldState->CheckIndex(mKey.c_str(), Index, mArgs);
}

