#pragma once

#include "Parser/HTNParserBase.h"

#include <vector>

class HTNToken;
class HTNWorldState;

class HTNWorldStateParser final : public HTNParserBase<HTNWorldState>
{
public:
    explicit HTNWorldStateParser(const std::vector<HTNToken>& inWorldStateTokens);

    bool Parse(HTNWorldState& outWorldState) final;

private:
    bool ParseFact(HTNWorldState& outWorldState, unsigned int& ioPosition);

    bool ParseIdentifier(HTNAtom& outIdentifier, unsigned int& inPosition);
    bool ParseArgument(HTNAtom& outArgument, unsigned int& inPosition);
};

inline HTNWorldStateParser::HTNWorldStateParser(const std::vector<HTNToken>& inWorldStateTokens) : HTNParserBase(inWorldStateTokens)
{
}
