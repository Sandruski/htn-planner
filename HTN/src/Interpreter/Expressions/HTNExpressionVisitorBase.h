#pragma once

#include <any>

class HTNBinaryExpression;
class HTNLiteralExpression;

class HTNExpressionVisitorBase
{
public:
	virtual [[nodiscard]] bool Visit(const HTNBinaryExpression& inBinaryExpression, std::any& outResult) const = 0;
	virtual [[nodiscard]] bool Visit(const HTNLiteralExpression& inLiteralExpression, std::any& outResult) const = 0;
};
