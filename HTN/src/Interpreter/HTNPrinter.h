#pragma once

#include "Interpreter/Expressions/HTNExpressionVisitorBase.h"

#include <memory>
#include <string>

class HTNExpressionBase;

// Returns a string representing the abstract syntax tree of the expression
// Only used for debug purposes
class HTNPrinter final : public HTNExpressionVisitorBase
{
public:
	explicit HTNPrinter(std::unique_ptr<const HTNExpressionBase> inRoot);

	[[nodiscard]] bool Print(std::string& outText) const;

	[[nodiscard]] bool Visit(const HTNBinaryExpression& inBinaryExpression, std::any& outValue) const final;
	[[nodiscard]] bool Visit(const HTNLiteralExpression& inLiteralExpression, std::any& outValue) const final;

private:
	std::unique_ptr<const HTNExpressionBase> mRoot;
};

inline HTNPrinter::HTNPrinter(std::unique_ptr<const HTNExpressionBase> inRoot)
	: mRoot(std::move(inRoot))
{
}
