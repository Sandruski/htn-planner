#include "Interpreter/Expressions/HTNBinaryExpression.h"

#include "Interpreter/Expressions/HTNExpressionVisitorBase.h"

bool HTNBinaryExpression::Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outValue) const
{
	return inVisitor.Visit(*this, outValue);
}
