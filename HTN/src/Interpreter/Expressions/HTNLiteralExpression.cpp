#include "Interpreter/Expressions/HTNLiteralExpression.h"

#include "Interpreter/Expressions/HTNExpressionVisitorBase.h"

bool HTNLiteralExpression::Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outResult) const
{
	return inVisitor.Visit(*this, outResult);
}
