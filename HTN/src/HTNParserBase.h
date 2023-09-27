#pragma once

#include "HTNToken.h"

#include <vector>

#if HTN_DEBUG
#include <string>
#endif

class HTNParserBase
{
public:
    explicit HTNParserBase(const std::vector<HTNToken>& inTokens);

protected:
    const HTNToken* ParseToken(const HTNTokenType inTokenType, unsigned int& inPosition);

    const HTNToken* GetToken(const unsigned int inPosition) const;

private:
    std::vector<HTNToken> mTokens;

#if HTN_DEBUG
protected:
    void LogLastError() const;

private:
    std::string mLastErrorMessage;
    int         mLastErrorRow    = -1;
    int         mLastErrorColumn = -1;
#endif
};

inline HTNParserBase::HTNParserBase(const std::vector<HTNToken>& inTokens) : mTokens(inTokens)
{
}

inline const HTNToken* HTNParserBase::GetToken(const unsigned int inPosition) const
{
    return (inPosition < mTokens.size() ? &mTokens[inPosition] : nullptr);
}
