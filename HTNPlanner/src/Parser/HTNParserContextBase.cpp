#include "Parser/HTNParserContextBase.h"

#include "Helpers/HTNToken.h"

HTNParserContextBase::HTNParserContextBase(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
{
}

HTNParserContextBase::~HTNParserContextBase() = default;

const HTNToken* HTNParserContextBase::GetToken(const unsigned int inPosition) const
{
    return ((inPosition < mTokens.size()) ? &mTokens[inPosition] : nullptr);
}

std::size_t HTNParserContextBase::GetTokensSize() const
{
    return mTokens.size();
}
