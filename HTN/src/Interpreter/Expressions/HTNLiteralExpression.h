#pragma once

#include "Interpreter/Expressions/HTNExpressionBase.h"

class HTNLiteralExpression final : public HTNExpressionBase
{
	using Super = HTNExpressionBase;

public:
	explicit HTNLiteralExpression(const HTNToken& inToken);

	[[nodiscard]] bool Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outResult) const final;

	std::any GetValue() const;
};

inline HTNLiteralExpression::HTNLiteralExpression(const HTNToken& inToken)
	: Super(inToken)
{
}

inline std::any HTNLiteralExpression::GetValue() const
{
	return mToken.GetValue();
}
