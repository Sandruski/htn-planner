#include "Interpreter/AST/HTNCondition.h"

#include "Interpreter/AST/HTNNodeVisitorBase.h"

HTNCondition::HTNCondition()
{
}

HTNCondition::~HTNCondition() = default;

std::vector<std::shared_ptr<const HTNPrimitiveTask>> HTNCondition::Accept(const HTNNodeVisitorBase& inVisitor) const
{
	return inVisitor.Visit(*this);
}

std::string HTNCondition::ToString() const
{
	return "Invalid Condition";
}
