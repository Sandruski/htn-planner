#pragma once

#include "Interpreter/Expressions/HTNExpressionBase.h"

#include <memory>
#include <string>

class HTNBinaryExpression final : public HTNExpressionBase
{
	using Super = HTNExpressionBase;

public:
	explicit HTNBinaryExpression(std::unique_ptr<const HTNExpressionBase> inLeft, const HTNToken& inToken, std::unique_ptr<const HTNExpressionBase> inRight);

	bool [[nodiscard]] Accept(const HTNExpressionVisitorBase& inVisitor, std::any& outValue) const final;

	const HTNExpressionBase* GetLeftOperand() const;
	HTNTokenType GetOperatorType() const;
	const std::string& GetOperatorName() const;
	const HTNExpressionBase* GetRightOperand() const;

private:
	std::unique_ptr<const HTNExpressionBase> mLeft;
	std::unique_ptr<const HTNExpressionBase> mRight;
};

inline HTNBinaryExpression::HTNBinaryExpression(std::unique_ptr<const HTNExpressionBase> inLeft, const HTNToken& inToken, std::unique_ptr<const HTNExpressionBase> inRight)
	: Super(inToken), mLeft(std::move(inLeft)), mRight(std::move(inRight))
{
}

inline const HTNExpressionBase* HTNBinaryExpression::GetLeftOperand() const
{
	return mLeft.get();
}

inline HTNTokenType HTNBinaryExpression::GetOperatorType() const
{
	return mToken.GetType();
}

inline const std::string& HTNBinaryExpression::GetOperatorName() const
{
	return mToken.GetLexeme();
}

inline const HTNExpressionBase* HTNBinaryExpression::GetRightOperand() const
{
	return mRight.get();
}
