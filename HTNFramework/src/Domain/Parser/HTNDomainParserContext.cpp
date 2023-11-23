// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Parser/HTNDomainParserContext.h"

HTNDomainParserContext::HTNDomainParserContext(const std::vector<HTNToken>& inTokens, std::shared_ptr<const HTNDomainNode>& outDomainNode)
    : HTNParserContextBase(inTokens), mDomainNode(outDomainNode)
{
}
