#pragma once

#include "Interpreter/HTNToken.h"

#include <algorithm>
#include <memory>
#include <vector>

class HTNExpressionBase;

// Builds an abstract syntax tree
class HTNParser
{
public:
	explicit HTNParser(const std::vector<HTNToken>& inTokens);

	[[nodiscard]] bool Parse(std::unique_ptr<const HTNExpressionBase>& outRoot);

private:
	[[nodiscard]] bool Expression(std::unique_ptr<const HTNExpressionBase>& outNode);
	[[nodiscard]] bool Term(std::unique_ptr<const HTNExpressionBase>& outNode); // + -
	[[nodiscard]] bool Factor(std::unique_ptr<const HTNExpressionBase>& outNode); // * /
	[[nodiscard]] bool Primary(std::unique_ptr<const HTNExpressionBase>& outNode); // Number Parenthesis

	const HTNToken* GetToken() const;
	void AdvancePosition();

	std::vector<HTNToken> mTokens;
	unsigned int mPosition = 0;
};

inline HTNParser::HTNParser(const std::vector<HTNToken>& inTokens)
	: mTokens(inTokens)
{
}

inline const HTNToken* HTNParser::GetToken() const
{
	return (mPosition < mTokens.size() ? &mTokens[mPosition] : nullptr);
}

inline void HTNParser::AdvancePosition()
{
	mPosition = std::clamp(static_cast<int>(mPosition) + 1, 0, static_cast<int>(mTokens.size()) - 1);
}
