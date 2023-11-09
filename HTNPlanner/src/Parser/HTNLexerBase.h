#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class HTNAtom;
class HTNToken;
enum class HTNTokenType : unsigned int;

// Breaks a text into a series of tokens
// Reports all lexical errors of the text
class HTNLexerBase
{
public:
    virtual ~HTNLexerBase() = default;

    virtual bool Lex(const std::string& inText, std::vector<HTNToken>& outTokens) = 0;

protected:
    void LexIdentifier(std::vector<HTNToken>& outTokens, const std::unordered_map<std::string, HTNTokenType>& inKeywords);
    void LexNumber(std::vector<HTNToken>& outTokens);
    bool LexString(std::vector<HTNToken>& outTokens);
    void LexComment();

    void AddToken(const HTNTokenType inType, const std::string& inLexeme, const HTNAtom& inValue, std::vector<HTNToken>& outTokens) const;

    char GetCharacter(const unsigned int inLookAhead = 0) const;
    void AdvancePosition(const bool inIsNewLine = false);

    void Reset(const std::string& inText);

    std::string  mText;

    unsigned int mPosition = 0;
    unsigned int mRow      = 0;
    unsigned int mColumn   = 0;
};

inline char HTNLexerBase::GetCharacter(const unsigned int inOffset) const
{
    const unsigned int Position = mPosition + inOffset;
    if (Position <= mText.length())
    {
        return mText[Position];
    }

    return '\0';
}

inline void HTNLexerBase::AdvancePosition(const bool inIsNewLine)
{
    if (mPosition >= mText.length())
    {
        return;
    }

    ++mPosition;

    if (inIsNewLine)
    {
        ++mRow;
        mColumn = 0;
    }
    else
    {
        ++mColumn;
    }
}
