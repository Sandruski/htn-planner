#pragma once

#include "Interpreter/Expressions/HTNExpressionBase.h"

#include <string>

class HTNLiteralExpression final : public HTNExpressionBase
{
	using Super = HTNExpressionBase;

public:
	explicit HTNLiteralExpression(const HTNToken& inToken);

	[[nodiscard]] bool Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outValue) const final;

	std::any GetValue() const;
	const std::string& GetName() const;
};

inline HTNLiteralExpression::HTNLiteralExpression(const HTNToken& inToken)
	: Super(inToken)
{
}

inline std::any HTNLiteralExpression::GetValue() const
{
	return mToken.GetValue();
}

inline const std::string& HTNLiteralExpression::GetName() const
{
	return mToken.GetLexeme();
}
