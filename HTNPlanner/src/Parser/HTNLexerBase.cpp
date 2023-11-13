#include "Parser/HTNLexerBase.h"

#include "HTNLexerHelpers.h"
#include "Helpers/HTNToken.h"

#if HTN_DEBUG
#include <limits>
#endif

void HTNLexerBase::LexIdentifier(std::vector<HTNToken>& outTokens, const std::unordered_map<std::string, HTNTokenType>& inKeywords)
{
    OPTICK_EVENT("LexIdentifier");

    const unsigned int StartPosition = mPosition;

    AdvancePosition();

    for (char Character = GetCharacter(); HTNLexerHelpers::IsAlphaNumeric(Character); Character = GetCharacter())
    {
        AdvancePosition();
    }

    const unsigned int EndPosition = mPosition - StartPosition;
    const std::string  Lexeme      = mText.substr(StartPosition, EndPosition);
    const auto         It          = inKeywords.find(Lexeme);
    const HTNTokenType TokenType   = It != inKeywords.end() ? It->second : HTNTokenType::IDENTIFIER;
    if (TokenType == HTNTokenType::TRUE)
    {
        AddToken(TokenType, Lexeme, HTNAtom(true), outTokens);
    }
    else if (TokenType == HTNTokenType::FALSE)
    {
        AddToken(TokenType, Lexeme, HTNAtom(false), outTokens);
    }
    else
    {
        AddToken(TokenType, Lexeme, HTNAtom(Lexeme), outTokens);
    }
}

void HTNLexerBase::LexNumber(std::vector<HTNToken>& outTokens)
{
    OPTICK_EVENT("LexNumber");

    const unsigned int StartPosition = mPosition;

#ifdef HTN_DEBUG
    const unsigned int StartRow    = mRow;
    const unsigned int StartColumn = mColumn;
#endif

    AdvancePosition();

    // Check for more digits
    for (char Character = GetCharacter(); HTNLexerHelpers::IsDigit(Character); Character = GetCharacter())
    {
        AdvancePosition();
    }

    // Check for fractional part
    const char NextCharacter = GetCharacter(1);
    if (GetCharacter() == '.' && HTNLexerHelpers::IsDigit(NextCharacter))
    {
        AdvancePosition();

        // Check for more digits in fractional part
        for (char Character = GetCharacter(); HTNLexerHelpers::IsDigit(Character); Character = GetCharacter())
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
    OPTICK_EVENT("LexString");

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
    OPTICK_EVENT("LexComment");

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

void HTNLexerBase::Reset(const std::string& inText)
{
    mText     = inText;
    mPosition = 0;
    mRow      = 0;
    mColumn   = 0;
}
