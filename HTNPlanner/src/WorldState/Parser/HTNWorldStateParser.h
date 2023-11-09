#pragma once

#include "Parser/HTNParserBase.h"

#include <vector>

class HTNToken;
class HTNWorldState;

class HTNWorldStateParser final : public HTNParserBase<HTNWorldState>
{
public:
    bool Parse(const std::vector<HTNToken>& inTokens, HTNWorldState& outWorldState) final;

private:
    bool ParseFact(HTNWorldState& outWorldState, unsigned int& ioPosition);

    bool ParseIdentifier(HTNAtom& outIdentifier, unsigned int& inPosition);
    bool ParseArgument(HTNAtom& outArgument, unsigned int& inPosition);
};
