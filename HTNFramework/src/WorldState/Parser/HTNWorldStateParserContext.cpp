// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "WorldState/Parser/HTNWorldStateParserContext.h"

HTNWorldStateParserContext::HTNWorldStateParserContext(const std::vector<HTNToken>& inTokens, HTNWorldState& outWorldState)
    : HTNParserContextBase(inTokens), mWorldState(outWorldState)
{
}
