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

// Builds an abstract syntax tree from a series of tokens
// Reports the first syntax error of the tokens
class HTNParser
{
public:
    explicit HTNParser(const std::vector<HTNToken>& inTokens);

    std::shared_ptr<const HTNDomain> Parse();

private:
    std::shared_ptr<HTNDomain>        ParseDomain();
    std::shared_ptr<HTNAxiom>         ParseAxiom();
    std::shared_ptr<HTNMethod>        ParseMethod();
    std::shared_ptr<HTNBranch>        ParseBranch();
    std::shared_ptr<HTNConditionBase> ParseCondition();
    std::shared_ptr<HTNConditionBase> ParseSubCondition();
    std::shared_ptr<HTNTask>          ParseTask();
    std::unique_ptr<HTNValue>         ParseArgument();
    std::unique_ptr<HTNValue>         ParseIdentifier();
    std::unique_ptr<HTNValue>         ParseNumber();
    std::unique_ptr<HTNValue>         ParseString();
    const HTNToken*                   ParseToken(const HTNTokenType inTokenType);

    const HTNToken* GetToken() const;
    void            AdvancePosition();
    void            SetPosition(const unsigned int inPosition);

    std::vector<HTNToken> mTokens;
    unsigned int          mPosition = 0;
};

inline HTNParser::HTNParser(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
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

inline void HTNParser::SetPosition(const unsigned int inPosition)
{
    mPosition = std::clamp(static_cast<int>(inPosition), 0, static_cast<int>(mTokens.size()) - 1);
}
