// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "WorldState/Parser/HTNWorldStateLexer.h"

#include "Parser/HTNLexerHelpers.h"
#include "Parser/HTNTokenType.h"
#include "WorldState/Parser/HTNWorldStateLexerContext.h"

bool HTNWorldStateLexer::Lex(HTNWorldStateLexerContext& ioWorldStateLexerContext) const
{
    OPTICK_EVENT("LexWorldState");

    bool Result = true;

    for (char Character = ioWorldStateLexerContext.GetCharacter(); HTNLexerHelpers::IsValidCharacter(Character);
         Character      = ioWorldStateLexerContext.GetCharacter())
    {
        switch (Character)
        {
        case '(': {
            // Left parenthesis
            ioWorldStateLexerContext.AddToken(HTNAtom(), HTNTokenType::LEFT_PARENTHESIS HTN_LOG_ONLY(, std::string(1, Character)));
            ioWorldStateLexerContext.AdvancePosition();
            break;
        }
        case ')': {
            // Right parenthesis
            ioWorldStateLexerContext.AddToken(HTNAtom(), HTNTokenType::RIGHT_PARENTHESIS HTN_LOG_ONLY(, std::string(1, Character)));
            ioWorldStateLexerContext.AdvancePosition();
            break;
        }
        case '/': {
            static constexpr uint32 LookAhead     = 1;
            const char              NextCharacter = ioWorldStateLexerContext.GetCharacter(LookAhead);
            if (NextCharacter == '/')
            {
                // Comment
                LexComment(ioWorldStateLexerContext);
                break;
            }

#ifdef HTN_ENABLE_LOGGING
            const std::string Message = std::format("Expected '/' after [{}] for a comment", Character);
            HTNLexerHelpers::PrintError(Message, ioWorldStateLexerContext);
#endif
            Result = false;
            ioWorldStateLexerContext.AdvancePosition();
            break;
        }
        case '"': {
            // String
            Result = LexString(ioWorldStateLexerContext) && Result;
            break;
        }
        case ' ': {
            // Whitespace
            ioWorldStateLexerContext.AdvancePosition();
            break;
        }
        case '\n': {
// Newline
#ifdef HTN_ENABLE_LOGGING
            static constexpr bool IsNewLine = true;
#endif
            ioWorldStateLexerContext.AdvancePosition(HTN_LOG_ONLY(IsNewLine));
            break;
        }
        default: {
            if (HTNLexerHelpers::IsDigit(Character))
            {
                // Number
                LexNumber(ioWorldStateLexerContext);
                break;
            }
            else if (HTNLexerHelpers::IsLetter(Character))
            {
                // Identifier
                static const std::unordered_map<std::string, HTNTokenType> Keywords = {{"true", HTNTokenType::TRUE}, {"false", HTNTokenType::FALSE}};
                LexIdentifier(Keywords, ioWorldStateLexerContext);
                break;
            }

#ifdef HTN_ENABLE_LOGGING
            const std::string Message = std::format("Character [{}] not recognized", HTNLexerHelpers::GetSpecialCharacterEscapeSequence(Character));
            HTNLexerHelpers::PrintError(Message, ioWorldStateLexerContext);
#endif
            Result = false;
            ioWorldStateLexerContext.AdvancePosition();
        }
        }
    }

    ioWorldStateLexerContext.AddToken(HTNAtom(), HTNTokenType::END_OF_FILE HTN_LOG_ONLY(, ""));

    return Result;
}
