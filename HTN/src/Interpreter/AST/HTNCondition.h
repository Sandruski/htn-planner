#pragma once

#include "Interpreter/AST/HTNNodeBase.h"
#include "Runtime/HTNDecompositionContext.h"
#include "Runtime/HTNWorldState.h"

#include <array>
#include <memory>
#include <string>
#include <type_traits>

class HTNAtom;
class HTNDecompositionContext;
class HTNValue;

class HTNConditionWorldStateQuery
{
public:
	HTNConditionWorldStateQuery() = default;
	explicit HTNConditionWorldStateQuery(const std::string& inKey, const std::vector<HTNAtom*>& inArguments);

	bool Check(const HTNWorldState& inWorldState, const int inIndex) const;

	void SetKey(const std::string& inKey);
	void AddArgument(HTNAtom& inArgument);

private:
	std::string mKey;
	std::vector<HTNAtom*> mArguments;
};

class HTNConditionBase : public HTNNodeBase
{
public:
	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;

	// Check if the condition is true.
	virtual bool Check(HTNDecompositionContext& ioDecompositionContext) const = 0;
};

class HTNCondition final : public HTNConditionBase, public std::enable_shared_from_this<HTNCondition>
{
public:
	explicit HTNCondition(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments);

	std::string ToString() const final;
	bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

	std::string GetName() const;

private:
	std::unique_ptr<const HTNValue> mName;
	std::vector<std::shared_ptr<const HTNValue>> mArguments;
};

class HTNConditionAnd final : public HTNConditionBase
{
public:
	explicit HTNConditionAnd(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions);

	std::string ToString() const final;
	bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

private:
	std::vector<std::shared_ptr<const HTNConditionBase>> mConditions;
};

class HTNConditionOr final : public HTNConditionBase
{
public:
	explicit HTNConditionOr(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions);

	std::string ToString() const final;
	bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

private:
	std::vector<std::shared_ptr<const HTNConditionBase>> mConditions;
};

class HTNConditionNot final : public HTNConditionBase
{
public:
	explicit HTNConditionNot(const std::shared_ptr<const HTNConditionBase>& inCondition);

	std::string ToString() const final;
	bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

private:
	std::shared_ptr<const HTNConditionBase> mCondition;
};

inline HTNConditionWorldStateQuery::HTNConditionWorldStateQuery(const std::string& inKey, const std::vector<HTNAtom*>& inArguments)
	: mKey(inKey), mArguments(inArguments)
{
}

inline void HTNConditionWorldStateQuery::SetKey(const std::string& inKey)
{
	mKey = inKey;
}

inline void HTNConditionWorldStateQuery::AddArgument(HTNAtom& inArgument)
{
	mArguments.emplace_back(&inArgument);
}
