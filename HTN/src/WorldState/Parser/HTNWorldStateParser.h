#pragma once

#include "HTNParserBase.h"

#include <vector>

class HTNToken;
class HTNWorldState;

class HTNWorldStateParser final : public HTNParserBase
{
public:
    explicit HTNWorldStateParser(const std::vector<HTNToken>& inWorldStateTokens);

    bool Parse(HTNWorldState& ioWorldState);

private:
    bool ParseFact(HTNWorldState& ioWorldState, unsigned int& ioPosition);

    const HTNAtom* ParseIdentifier(unsigned int& inPosition);
    const HTNAtom* ParseArgument(unsigned int& inPosition);
};

inline HTNWorldStateParser::HTNWorldStateParser(const std::vector<HTNToken>& inWorldStateTokens) : HTNParserBase(inWorldStateTokens)
{
}
