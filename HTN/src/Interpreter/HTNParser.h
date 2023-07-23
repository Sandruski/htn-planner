#pragma once

#include "Interpreter/HTNToken.h"

#include <algorithm>
#include <memory>
#include <vector>

class HTNBranch;
class HTNConditionBase;
class HTNDomain;
class HTNMethod;
class HTNCondition;
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
	std::shared_ptr<const HTNBranch> ParseBranch(const std::shared_ptr<const HTNMethod>& inMethod);
	std::shared_ptr<const HTNConditionBase> ParseCondition();
	std::shared_ptr<const HTNConditionBase> ParseSubCondition();
	std::shared_ptr<const HTNTask> ParseTask(const std::shared_ptr<const HTNMethod>& inMethod);
	std::unique_ptr<const HTNValue> ParseArgument();
	std::unique_ptr<const HTNValue> ParseIdentifier();
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
