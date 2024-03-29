// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Parser/HTNLexerBase.h"

#include "HTNLexerHelpers.h"
#include "Parser/HTNLexerContextBase.h"
#include "Parser/HTNTokenType.h"

#ifdef HTN_ENABLE_LOGGING
#include <limits>
#endif

HTNLexerBase::~HTNLexerBase() = default;

void HTNLexerBase::LexIdentifier(const std::unordered_map<std::string, HTNTokenType>& inKeywords, HTNLexerContextBase& ioLexerContext) const
{
    OPTICK_EVENT("LexIdentifier");

    const uint32 StartPosition = ioLexerContext.GetPosition();

    ioLexerContext.AdvancePosition();

    for (char Character = ioLexerContext.GetCharacter(); HTNLexerHelpers::IsAlphanumeric(Character); Character = ioLexerContext.GetCharacter())
    {
        ioLexerContext.AdvancePosition();
    }

    const uint32       CurrentPosition = ioLexerContext.GetPosition();
    const uint32       EndPosition     = CurrentPosition - StartPosition;
    const std::string& Text            = ioLexerContext.GetText();
    const std::string  Lexeme          = Text.substr(StartPosition, EndPosition);
    const auto         It              = inKeywords.find(Lexeme);
    const HTNTokenType TokenType       = (It != inKeywords.cend()) ? It->second : HTNTokenType::IDENTIFIER;
    if (TokenType == HTNTokenType::TRUE)
    {
        ioLexerContext.AddToken(true, TokenType HTN_LOG_ONLY(, Lexeme));
    }
    else if (TokenType == HTNTokenType::FALSE)
    {
        ioLexerContext.AddToken(false, TokenType HTN_LOG_ONLY(, Lexeme));
    }
    else
    {
        ioLexerContext.AddToken(Lexeme, TokenType HTN_LOG_ONLY(, Lexeme));
    }
}

void HTNLexerBase::LexNumber(HTNLexerContextBase& ioLexerContext) const
{
    OPTICK_EVENT("LexNumber");

    const uint32 StartPosition = ioLexerContext.GetPosition();

#ifdef HTN_ENABLE_LOGGING
    const uint32 StartRow    = ioLexerContext.GetRow();
    const uint32 StartColumn = ioLexerContext.GetColumn();
#endif

    ioLexerContext.AdvancePosition();

    // Check for more digits
    for (char Character = ioLexerContext.GetCharacter(); HTNLexerHelpers::IsDigit(Character); Character = ioLexerContext.GetCharacter())
    {
        ioLexerContext.AdvancePosition();
    }

    const uint32       CurrentPosition = ioLexerContext.GetPosition();
    const std::string& Text            = ioLexerContext.GetText();

    // Check for fractional part
    static constexpr uint32 LookAhead     = 1;
    const char              NextCharacter = ioLexerContext.GetCharacter(LookAhead);
    if (ioLexerContext.GetCharacter() == '.' && HTNLexerHelpers::IsDigit(NextCharacter))
    {
        ioLexerContext.AdvancePosition();

        // Check for more digits in fractional part
        for (char Character = ioLexerContext.GetCharacter(); HTNLexerHelpers::IsDigit(Character); Character = ioLexerContext.GetCharacter())
        {
            ioLexerContext.AdvancePosition();
        }

        const uint32      EndPosition = CurrentPosition - StartPosition;
        const std::string Lexeme      = Text.substr(StartPosition, EndPosition);
        const float       Number      = std::stof(Lexeme);
        HTN_DOMAIN_CLOG_ERROR(std::stod(Lexeme) < std::numeric_limits<float>::min() || std::stod(Lexeme) > std::numeric_limits<float>::max(),
                              StartRow, StartColumn, "Number out of bounds");
        ioLexerContext.AddToken(Number, HTNTokenType::NUMBER HTN_LOG_ONLY(, Lexeme));
    }
    else
    {
        const uint32      EndPosition = CurrentPosition - StartPosition;
        const std::string Lexeme      = Text.substr(StartPosition, EndPosition);
        const int32       Number      = std::stoi(Lexeme);
        ioLexerContext.AddToken(Number, HTNTokenType::NUMBER HTN_LOG_ONLY(, Lexeme));
    }
}

bool HTNLexerBase::LexString(HTNLexerContextBase& ioLexerContext) const
{
    OPTICK_EVENT("LexString");

    const uint32 StartPosition = ioLexerContext.GetPosition();

    ioLexerContext.AdvancePosition();

    for (char Character = ioLexerContext.GetCharacter(); Character != '"'; Character = ioLexerContext.GetCharacter())
    {
        ioLexerContext.AdvancePosition();
    }

    if (ioLexerContext.GetCharacter() != '"')
    {
#ifdef HTN_ENABLE_LOGGING
        const uint32 Row    = ioLexerContext.GetRow();
        const uint32 Column = ioLexerContext.GetColumn();
#endif
        HTN_DOMAIN_LOG_ERROR(Row, Column, "Character '\"' could not be found at end of the string");
        return false;
    }

    const uint32 CurrentPosition = ioLexerContext.GetPosition();
    const uint32 EndPosition     = CurrentPosition - StartPosition;

    ioLexerContext.AdvancePosition();

    const std::string& Text   = ioLexerContext.GetText();
    const std::string  Lexeme = Text.substr(StartPosition + 1, EndPosition - 1);
    ioLexerContext.AddToken(Lexeme, HTNTokenType::STRING HTN_LOG_ONLY(, Lexeme));

    return true;
}

void HTNLexerBase::LexComment(HTNLexerContextBase& ioLexerContext) const
{
    OPTICK_EVENT("LexComment");

    for (char Character = ioLexerContext.GetCharacter(); Character != '\n'; Character = ioLexerContext.GetCharacter())
    {
        ioLexerContext.AdvancePosition();
    }

#ifdef HTN_ENABLE_LOGGING
    static constexpr bool IsNewLine = true;
#endif
    ioLexerContext.AdvancePosition(HTN_LOG_ONLY(IsNewLine));
}
