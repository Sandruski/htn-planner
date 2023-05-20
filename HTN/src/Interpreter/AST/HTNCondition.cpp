#include "Interpreter/AST/HTNCondition.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"
#include "Interpreter/AST/HTNValue.h"

bool HTNConditionWorldStateQuery::Check(const HTNWorldState& inWorldState, const int inIndex) const
{
	return inWorldState.CheckIndex(mKey.c_str(), inIndex, mArguments);
}

std::vector<std::shared_ptr<const HTNTask>> HTNConditionBase::Accept(const HTNNodeVisitorBase& inVisitor) const
{
	return inVisitor.Visit(*this);
}

HTNCondition::HTNCondition(std::unique_ptr<const HTNValue> inName, const std::vector<std::shared_ptr<const HTNValue>>& inArguments)
	: mName(std::move(inName)), mArguments(inArguments)
{
}

std::string HTNCondition::ToString() const
{
	return GetName();
}

bool HTNCondition::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	const HTNAtom& NameValue = mName->GetValue();
	const std::string& Key = NameValue.GetValue<std::string>();

	std::vector<HTNAtom*> Arguments;
	for (const std::shared_ptr<const HTNValue>& Argument : mArguments)
	{
		const HTNAtom& ArgumentValue = Argument->GetValue();
		const std::string& VariableName = ArgumentValue.GetValue<std::string>();
		HTNAtom& Variable = ioDecompositionContext.AddOrGetVariable(VariableName);
		Arguments.emplace_back(&Variable);
	}

	const HTNConditionWorldStateQuery Condition = HTNConditionWorldStateQuery(Key, Arguments);
	const HTNWorldState* WorldState = ioDecompositionContext.GetWorldState();
	const int Index = ioDecompositionContext.IncrementIndex(shared_from_this());
	return Condition.Check(*WorldState, Index);
}

std::string HTNCondition::GetName() const
{
	return mName ? mName->ToString() : "Invalid Condition";
}

HTNConditionAnd::HTNConditionAnd(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions)
	: mConditions(inConditions)
{
}

std::string HTNConditionAnd::ToString() const
{
	// TODO salvarez Print names of conditions AND
	return "AND Condition";
}

bool HTNConditionAnd::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	for (const std::shared_ptr<const HTNConditionBase>& Condition : mConditions)
	{
		if (!Condition->Check(ioDecompositionContext))
		{
			return false;
		}
	}

	return true;
}

HTNConditionOr::HTNConditionOr(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions)
	: mConditions(inConditions)
{
}

std::string HTNConditionOr::ToString() const
{
	// TODO salvarez Print names of conditions OR
	return "OR Condition";
}

bool HTNConditionOr::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	for (const std::shared_ptr<const HTNConditionBase>& Condition : mConditions)
	{
		if (Condition->Check(ioDecompositionContext))
		{
			return true;
		}
	}

	return false;
}

HTNConditionNot::HTNConditionNot(const std::shared_ptr<const HTNConditionBase>& inCondition)
	: mCondition(inCondition)
{
}

std::string HTNConditionNot::ToString() const
{
	// TODO salvarez Print names of conditions !
	return "NOT Condition";
}

bool HTNConditionNot::Check(HTNDecompositionContext& ioDecompositionContext) const
{
	return !mCondition->Check(ioDecompositionContext);
}
