#include "HTNParserBase.h"

#if HTN_DEBUG
#include "HTNLog.h"

#include <format>
#endif

const HTNToken* HTNParserBase::ParseToken(const HTNTokenType inTokenType, unsigned int& inPosition)
{
    const HTNToken* Token = GetToken(inPosition);
    if (!Token)
    {
#if HTN_DEBUG
        mLastErrorMessage = std::format("Token at [{}] is out of bounds [{}]", inPosition, mTokens.size());
        mLastErrorRow     = mLastErrorColumn - 1;
#endif
        return nullptr;
    }

    const HTNTokenType TokenType = Token->GetType();
    if (inTokenType != TokenType)
    {
#if HTN_DEBUG
        mLastErrorMessage = std::format("Token [{}] is of type [{}] instead of [{}]", Token->GetLexeme(), GetTokenTypeString(Token->GetType()),
                                        GetTokenTypeString(inTokenType));
        mLastErrorRow     = Token->GetRow();
        mLastErrorColumn  = Token->GetColumn();
#endif
        return nullptr;
    }

    ++inPosition;

    return Token;
}

#if HTN_DEBUG
void HTNParserBase::LogLastError() const
{
    LOG_HTN_ERROR(mLastErrorRow, mLastErrorColumn, "{}", mLastErrorMessage);
}
#endif
