#include "Interpreter/Expressions/HTNBinaryExpression.h"

#include "Interpreter/Expressions/HTNExpressionVisitorBase.h"

bool HTNBinaryExpression::Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outResult) const
{
	return inVisitor.Visit(*this, outResult);
}
