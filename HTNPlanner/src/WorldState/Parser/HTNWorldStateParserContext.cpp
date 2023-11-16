#include "WorldState/Parser/HTNWorldStateParserContext.h"

HTNWorldStateParserContext::HTNWorldStateParserContext(const std::vector<HTNToken>& inTokens, HTNWorldState& outWorldState)
    : HTNParserContextBase(inTokens), mWorldState(outWorldState)
{
}
