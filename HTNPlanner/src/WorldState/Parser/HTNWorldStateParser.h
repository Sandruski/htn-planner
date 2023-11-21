// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"
#include "Parser/HTNParserBase.h"

class HTNAtom;
class HTNWorldState;
class HTNWorldStateParserContext;

class HTNWorldStateParser final : public HTNParserBase
{
public:
    bool Parse(HTNWorldStateParserContext& ioWorldStateParserContext) const;

private:
    bool ParseFact(HTNWorldStateParserContext& ioWorldStateParserContext, HTNWorldState& outWorldState) const;

    bool ParseIdentifier(HTNWorldStateParserContext& ioWorldStateParserContext, HTNAtom& outIdentifier) const;
    bool ParseArgument(HTNWorldStateParserContext& ioWorldStateParserContext, HTNAtom& outArgument) const;
};
