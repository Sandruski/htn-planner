#include "Interpreter/Expressions/HTNLiteralExpression.h"

#include "Interpreter/Expressions/HTNExpressionVisitorBase.h"

bool HTNLiteralExpression::Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outValue) const
{
	return inVisitor.Visit(*this, outValue);
}
