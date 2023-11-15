#include "Parser/HTNParserContextBase.h"

HTNParserContextBase::HTNParserContextBase(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
{
}

HTNParserContextBase::~HTNParserContextBase() = default;
