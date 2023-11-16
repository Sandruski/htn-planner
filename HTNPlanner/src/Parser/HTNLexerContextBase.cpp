#include "Parser/HTNLexerContextBase.h"

#include "Helpers/HTNToken.h"

HTNLexerContextBase::HTNLexerContextBase(const std::string& inText, std::vector<HTNToken>& outTokens) : mText(inText), mTokens(outTokens)
{
}

HTNLexerContextBase::~HTNLexerContextBase() = default;

void HTNLexerContextBase::AddToken(const HTNAtom& inValue, const HTNTokenType inType HTN_DEBUG_ONLY(, const std::string& inLexeme))
{
    mTokens.emplace_back(inValue, inType HTN_DEBUG_ONLY(, inLexeme, mRow, mColumn));
}

char HTNLexerContextBase::GetCharacter(const unsigned int inOffset) const
{
    const unsigned int Position = mPosition + inOffset;
    if (Position <= mText.length())
    {
        return mText[Position];
    }

    return '\0';
}

void HTNLexerContextBase::AdvancePosition(const bool inIsNewLine)
{
    if (mPosition >= mText.length())
    {
        return;
    }

    ++mPosition;

#ifdef HTN_DEBUG
    if (inIsNewLine)
    {
        ++mRow;
        mColumn = 0;
    }
    else
    {
        ++mColumn;
    }
#endif
}
