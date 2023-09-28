#include "Parser/HTNLexerBase.h"

#include "HTNAtom.h"
#include "HTNLexerHelpers.h"
#include "HTNLog.h"
#include "HTNToken.h"

#if HTN_DEBUG
#include <limits>
#endif

void HTNLexerBase::LexIdentifier(std::vector<HTNToken>& outTokens, const std::unordered_map<std::string, HTNTokenType>& inKeywords)
{
    const unsigned int StartPosition = mPosition;

    AdvancePosition();

    for (char Character = GetCharacter(); HTNLexer::Helpers::IsAlphaNumeric(Character); Character = GetCharacter())
    {
        AdvancePosition();
    }

    const unsigned int EndPosition = mPosition - StartPosition;
    const std::string  Lexeme      = mText.substr(StartPosition, EndPosition);
    const auto         It          = inKeywords.find(Lexeme);
    const HTNTokenType TokenType   = It != inKeywords.end() ? It->second : HTNTokenType::IDENTIFIER;
    AddToken(TokenType, Lexeme, HTNAtom(Lexeme), outTokens);
}

void HTNLexerBase::LexNumber(std::vector<HTNToken>& outTokens)
{
    const unsigned int StartPosition = mPosition;

#ifdef HTN_DEBUG
    const unsigned int StartRow    = mRow;
    const unsigned int StartColumn = mColumn;
#endif

    AdvancePosition();

    // Check for more digits
    for (char Character = GetCharacter(); HTNLexer::Helpers::IsDigit(Character); Character = GetCharacter())
    {
        AdvancePosition();
    }

    // Check for fractional part
    const char NextCharacter = GetCharacter(1);
    if (GetCharacter() == '.' && HTNLexer::Helpers::IsDigit(NextCharacter))
    {
        AdvancePosition();

        // Check for more digits in fractional part
        for (char Character = GetCharacter(); HTNLexer::Helpers::IsDigit(Character); Character = GetCharacter())
        {
            AdvancePosition();
        }

        const unsigned int EndPosition = mPosition - StartPosition;
        const std::string  Lexeme      = mText.substr(StartPosition, EndPosition);
        const float        Number      = std::stof(Lexeme);
        CLOG_HTN_ERROR(std::stod(Lexeme) < std::numeric_limits<float>::min() || std::stod(Lexeme) > std::numeric_limits<float>::max(), StartRow,
                       StartColumn, "Number out of bounds");
        AddToken(HTNTokenType::NUMBER, Lexeme, HTNAtom(Number), outTokens);
    }
    else
    {
        const unsigned int EndPosition = mPosition - StartPosition;
        const std::string  Lexeme      = mText.substr(StartPosition, EndPosition);
        const int          Number      = std::stoi(Lexeme);
        AddToken(HTNTokenType::NUMBER, Lexeme, HTNAtom(Number), outTokens);
    }
}

bool HTNLexerBase::LexString(std::vector<HTNToken>& outTokens)
{
    const unsigned int StartPosition = mPosition;

    AdvancePosition();

    for (char Character = GetCharacter(); Character != '"'; Character = GetCharacter())
    {
        AdvancePosition();
    }

    if (GetCharacter() != '"')
    {
        LOG_HTN_ERROR(mRow, mColumn, "Character '\"' not found at end of string");
        return false;
    }

    const unsigned int EndPosition = mPosition - StartPosition;

    AdvancePosition();

    const std::string Lexeme = mText.substr(StartPosition + 1, EndPosition - 1);
    AddToken(HTNTokenType::STRING, Lexeme, HTNAtom(Lexeme), outTokens);

    return true;
}

void HTNLexerBase::LexComment()
{
    for (char Character = GetCharacter(); Character != '\n'; Character = GetCharacter())
    {
        AdvancePosition();
    }

    AdvancePosition(true);
}

void HTNLexerBase::AddToken(const HTNTokenType inType, const std::string& inLexeme, const HTNAtom& inValue, std::vector<HTNToken>& outTokens) const
{
    outTokens.emplace_back(inType, inLexeme, inValue, mRow, mColumn);
}