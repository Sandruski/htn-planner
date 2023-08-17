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
class HTNValue;
class HTNAxiom;
class HTNNodeBase;

// Builds an abstract syntax tree from a series of tokens
// Reports the first syntax error of the tokens
class HTNParser
{
public:
    explicit HTNParser(const std::vector<HTNToken>& inTokens);

    std::shared_ptr<const HTNDomain> Parse();

private:
    std::shared_ptr<HTNDomain>        ParseDomain(unsigned int& inPosition);
    std::shared_ptr<HTNAxiom>         ParseAxiom(unsigned int& inPosition);
    std::shared_ptr<HTNMethod>        ParseMethod(unsigned int& inPosition);
    std::shared_ptr<HTNBranch>        ParseBranch(unsigned int& inPosition, const std::shared_ptr<HTNMethod>& inParent);
    std::shared_ptr<HTNConditionBase> ParseCondition(unsigned int& inPosition, const std::shared_ptr<HTNNodeBase>& inParent);
    std::shared_ptr<HTNConditionBase> ParseSubCondition(unsigned int& inPosition, const std::shared_ptr<HTNNodeBase>& inParent);
    std::shared_ptr<HTNTask>          ParseTask(unsigned int& inPosition);
    std::unique_ptr<HTNValue>         ParseArgument(unsigned int& inPosition);
    std::unique_ptr<HTNValue>         ParseIdentifier(unsigned int& inPosition);
    std::unique_ptr<HTNValue>         ParseNumber(unsigned int& inPosition);
    std::unique_ptr<HTNValue>         ParseString(unsigned int& inPosition);
    const HTNToken*                   ParseToken(unsigned int& inPosition, const HTNTokenType inTokenType);

    const HTNToken* GetToken(const unsigned int inPosition) const;

    std::vector<HTNToken> mTokens;
};

inline HTNParser::HTNParser(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
{
}

inline const HTNToken* HTNParser::GetToken(const unsigned int inPosition) const
{
    return (inPosition < mTokens.size() ? &mTokens[inPosition] : nullptr);
}
