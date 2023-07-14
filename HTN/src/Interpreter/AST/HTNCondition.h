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

/**
 * Condition world state query
 * - Queries the arguments in the database
 * - Binds unbound arguments
 */
class HTNConditionWorldStateQuery
{
public:
	HTNConditionWorldStateQuery() = default;
	explicit HTNConditionWorldStateQuery(const std::string& inKey, const std::vector<HTNAtom*>& inArguments);
	
	// Check if the condition is true
	bool Check(const HTNWorldState& inWorldState, const int inIndex) const;

	void SetKey(const std::string& inKey);
	void AddArgument(HTNAtom& inArgument);

private:
	std::string mKey;
	std::vector<HTNAtom*> mArguments;
};

/**
 * Base condition
 * - HTNCondition
 * - HTNConditionAnd
 * - HTNConditionOr
 * - HTNConditionAlt
 * - HTNConditionNot
 */
class HTNConditionBase : public HTNNodeBase
{
public:
	std::vector<std::shared_ptr<const HTNTask>> Accept(const HTNNodeVisitorBase& inVisitor) const final;

	virtual bool Check(HTNDecompositionContext& ioDecompositionContext) const = 0;
};

/**
 * Condition
 * - Queries the arguments in the database
 * - Binds unbound arguments
 * // TODO salvarez Force to have an AND at least, we cannot have this condition alone
 */
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

/**
 * AND condition
 * - sub-condition && sub-condition && ...
 * - In order to succeed, all of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of its sub-conditions
 * - If it binds any unbound arguments of a sub-condition, the sub-condition is considered for backtracking
 * - If it succeeds, the arguments of its sub-conditions will be bound
 */
class HTNConditionAnd final : public HTNConditionBase, public std::enable_shared_from_this<HTNConditionAnd>
{
public:
	explicit HTNConditionAnd(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions);

	std::string ToString() const final;
	bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

private:
	std::vector<std::shared_ptr<const HTNConditionBase>> mSubConditions;
};

/**
 * OR condition
 * - sub-condition || sub-condition || ...
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - The successful sub-condition is not considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound // TODO salvarez Detect this case during parsing
 */
class HTNConditionOr final : public HTNConditionBase, public std::enable_shared_from_this<HTNConditionOr>
{
public:
	explicit HTNConditionOr(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions);

	std::string ToString() const final;
	bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

private:
	std::vector<std::shared_ptr<const HTNConditionBase>> mSubConditions;
};

/**
 * ALT condition
 * - sub-condition || sub-condition || ...
 * - In order to succeed, one of its sub-conditions must succeed
 * - If it succeeds, it binds the unbound arguments of the successful sub-condition
 * - If it binds any unbound arguments of the successful sub-condition, the successful sub-condition is considered for backtracking
 * - If it succeeds, the arguments that were already bound and the arguments of the successful sub-condition will be bound // TODO salvarez Detect this case during parsing
 */
class HTNConditionAlt final : public HTNConditionBase, public std::enable_shared_from_this<HTNConditionAlt>
{
public:
    explicit HTNConditionAlt(const std::vector<std::shared_ptr<const HTNConditionBase>>& inSubConditions);

    std::string ToString() const final;
    bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

private:
    std::vector<std::shared_ptr<const HTNConditionBase>> mSubConditions;
};

/**
 * NOT condition
 * - !sub-condition
 * - In order to succeed, its sub-condition must fail
 * - It does not bind the unbound arguments of its sub-condition
 * - Its sub-condition is not considered for backtracking
 * - If it succeeds, only the arguments that were already bound will still be bound // TODO salvarez Detect this case during parsing
 */
class HTNConditionNot final : public HTNConditionBase
{
public:
	explicit HTNConditionNot(const std::shared_ptr<const HTNConditionBase>& inSubCondition);

	std::string ToString() const final;
	bool Check(HTNDecompositionContext& ioDecompositionContext) const final;

private:
	std::shared_ptr<const HTNConditionBase> mSubCondition;
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
