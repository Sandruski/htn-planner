#pragma once

#include "Interpreter/HTNToken.h"

#include <algorithm>
#include <memory>
#include <vector>

class HTNBranch;
class HTNCondition;
class HTNDomain;
class HTNMethod;
class HTNTask;
class HTNTask;
class HTNValue;

// Builds an abstract syntax tree from a series of tokens
// Reports the first syntax error of the tokens
class HTNParser
{
public:
	explicit HTNParser(const std::vector<HTNToken>& inTokens);

	std::shared_ptr<const HTNDomain> Parse();

private:
	std::shared_ptr<const HTNDomain> ParseDomain();
	std::shared_ptr<const HTNMethod> ParseMethod();
	std::shared_ptr<const HTNBranch> ParseBranch();
	std::shared_ptr<const HTNCondition> ParseCondition();
	std::shared_ptr<const HTNTask> ParseTask();
	std::unique_ptr<const HTNValue> ParseIdentifier();
	std::unique_ptr<const HTNValue> ParseSymbol();
	std::unique_ptr<const HTNValue> ParseNumber();
	std::unique_ptr<const HTNValue> ParseString();
	const HTNToken* ParseToken(const HTNTokenType inTokenType);

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
