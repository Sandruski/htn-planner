#include "Domain/Parser/HTNDomainParserContext.h"

HTNDomainParserContext::HTNDomainParserContext(const std::vector<HTNToken>& inTokens, std::shared_ptr<const HTNDomainNode>& outDomainNode)
    : HTNParserContextBase(inTokens), mDomainNode(outDomainNode)
{
}
