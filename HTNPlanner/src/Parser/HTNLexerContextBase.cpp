#include "Parser/HTNLexerContextBase.h"

HTNLexerContextBase::HTNLexerContextBase(const std::string& inText) : mText(inText)
{
}

HTNLexerContextBase::~HTNLexerContextBase() = default;

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
