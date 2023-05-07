#include "Interpreter/AST/HTNCondition.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"

std::vector<std::shared_ptr<const HTNTask>> HTNConditionBase::Accept(const HTNNodeVisitorBase& inVisitor) const
{
	return inVisitor.Visit(*this);
}

std::string HTNConditionBase::ToString() const
{
	return "Invalid Condition";
}

HTNConditionAnd::HTNConditionAnd(const std::vector<std::shared_ptr<const HTNConditionBase>>& inConditions)
	: mConditions(inConditions)
{
}

bool HTNConditionAnd::Check(HTNDecompositionContext& ioContext) const
{
	for (const std::shared_ptr<const HTNConditionBase>& Condition : mConditions)
	{
		if (!Condition->Check(ioContext))
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

bool HTNConditionOr::Check(HTNDecompositionContext& ioContext) const
{
	for (const std::shared_ptr<const HTNConditionBase>& Condition : mConditions)
	{
		if (Condition->Check(ioContext))
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

bool HTNConditionNot::Check(HTNDecompositionContext& ioContext) const
{
	return !mCondition->Check(ioContext);
}
