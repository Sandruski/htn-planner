#pragma once

#include "Interpreter/Expressions/HTNExpressionVisitorBase.h"

#include <memory>

class HTNExpressionBase;

class HTNInterpreter final : public HTNExpressionVisitorBase
{
public:
	explicit HTNInterpreter(std::unique_ptr<const HTNExpressionBase> inRoot);

	[[nodiscard]] bool Interpret(std::any& outResult) const;

	[[nodiscard]] bool Visit(const HTNBinaryExpression& inBinaryExpression, std::any& outResult) const final;
	[[nodiscard]] bool Visit(const HTNLiteralExpression& inLiteralExpression, std::any& outResult) const final;

private:
	std::unique_ptr<const HTNExpressionBase> mRoot;
};

inline HTNInterpreter::HTNInterpreter(std::unique_ptr<const HTNExpressionBase> inRoot)
	: mRoot(std::move(inRoot))
{
}
