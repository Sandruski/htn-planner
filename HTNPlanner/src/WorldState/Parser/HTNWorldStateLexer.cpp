#include "WorldState/Parser/HTNWorldStateLexer.h"

#include "Helpers/HTNToken.h"
#include "Parser/HTNLexerHelpers.h"
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
            ioWorldStateLexerContext.AddToken(HTNAtom(), HTNTokenType::LEFT_PARENTHESIS HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioWorldStateLexerContext.AdvancePosition();
            break;
        }
        case ')': {
            // Right parenthesis
            ioWorldStateLexerContext.AddToken(HTNAtom(), HTNTokenType::RIGHT_PARENTHESIS HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioWorldStateLexerContext.AdvancePosition();
            break;
        }
        case '/': {
            const char NextCharacter = ioWorldStateLexerContext.GetCharacter(1);
            if (NextCharacter == '/')
            {
                // Comment
                LexComment(ioWorldStateLexerContext);
                break;
            }

#ifdef HTN_DEBUG
            const std::string Message = std::format("Expected '/' after [{}] for a comment", Character);
            HTNLexerHelpers::PrintErrorMessage(Message, ioWorldStateLexerContext);
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
            static constexpr bool IsNewLine = true;
            ioWorldStateLexerContext.AdvancePosition(IsNewLine);
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

#ifdef HTN_DEBUG
            const std::string Message = std::format("Character [{}] not recognized", HTNLexerHelpers::GetSpecialCharacterEscapeSequence(Character));
            HTNLexerHelpers::PrintErrorMessage(Message, ioWorldStateLexerContext);
#endif
            Result = false;
            ioWorldStateLexerContext.AdvancePosition();
        }
        }
    }

    ioWorldStateLexerContext.AddToken(HTNAtom(), HTNTokenType::END_OF_FILE HTN_DEBUG_ONLY(, ""));

    return Result;
}
