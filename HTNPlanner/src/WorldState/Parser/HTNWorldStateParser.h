#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNParserBase.h"

class HTNAtom;
class HTNWorldState;
class HTNWorldStateParserContext;

class HTNWorldStateParser final : public HTNParserBase
{
public:
    bool Parse(HTNWorldStateParserContext& ioWorldStateParserContext) const;

private:
    bool ParseFact(HTNWorldState& outWorldState, HTNWorldStateParserContext& ioWorldStateParserContext) const;

    bool ParseIdentifier(HTNAtom& outIdentifier, HTNWorldStateParserContext& ioWorldStateParserContext) const;
    bool ParseArgument(HTNAtom& outArgument, HTNWorldStateParserContext& ioWorldStateParserContext) const;
};
