#pragma once

#include "HTNToken.h"

#include <vector>

#if HTN_DEBUG
#include <format>
#include <string>
#endif

template<typename T>
class HTNParserBase
{
public:
    explicit HTNParserBase(const std::vector<HTNToken>& inTokens);
    virtual ~HTNParserBase() = default;

    virtual bool Parse(T& outValue) = 0;

protected:
    const HTNToken* ParseToken(const HTNTokenType inTokenType, unsigned int& inPosition);

    const HTNToken* GetToken(const unsigned int inPosition) const;

    std::vector<HTNToken> mTokens;

#if HTN_DEBUG
protected:
    std::string mLastErrorMessage;
    int         mLastErrorRow    = -1;
    int         mLastErrorColumn = -1;
#endif
};

template<typename T>
inline HTNParserBase<T>::HTNParserBase(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
{
}

template<typename T>
const HTNToken* HTNParserBase<T>::ParseToken(const HTNTokenType inTokenType, unsigned int& inPosition)
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

template<typename T>
inline const HTNToken* HTNParserBase<T>::GetToken(const unsigned int inPosition) const
{
    return inPosition < mTokens.size() ? &mTokens[inPosition] : nullptr;
}