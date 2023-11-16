#include "Parser/HTNParserContextBase.h"

#include "Helpers/HTNToken.h"

HTNParserContextBase::HTNParserContextBase(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
{
}

HTNParserContextBase::~HTNParserContextBase() = default;

const HTNToken* HTNParserContextBase::GetToken(const uint32 inPosition) const
{
    return ((inPosition < mTokens.size()) ? &mTokens[inPosition] : nullptr);
}

size HTNParserContextBase::GetTokensSize() const
{
    return mTokens.size();
}
