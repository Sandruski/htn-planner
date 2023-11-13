#include "WorldState/Parser/HTNWorldStateLexer.h"

#include "Helpers/HTNToken.h"
#include "Parser/HTNLexerHelpers.h"

bool HTNWorldStateLexer::Lex(const std::string& inText, std::vector<HTNToken>& outTokens)
{
    OPTICK_EVENT("LexWorldState");

    Reset(inText);

    bool Result = true;

    for (char Character = GetCharacter(); HTNLexerHelpers::IsValidCharacter(Character); Character = GetCharacter())
    {
        switch (Character)
        {
        case '(': {
            // Left parenthesis
            AddToken(HTNTokenType::LEFT_PARENTHESIS, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case ')': {
            // Right parenthesis
            AddToken(HTNTokenType::RIGHT_PARENTHESIS, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '/': {
            const char NextCharacter = GetCharacter(1);
            if (NextCharacter == '/')
            {
                // Comment
                LexComment();
                break;
            }

            LOG_HTN_ERROR(mRow, mColumn, "Expected '/' after [{}] for a comment", Character);
            Result = false;
            AdvancePosition();
            break;
        }
        case '"': {
            // String
            Result = LexString(outTokens) && Result;
            break;
        }
        case ' ': {
            // Whitespace
            AdvancePosition();
            break;
        }
        case '\n': {
            // Newline
            constexpr bool IsNewLine = true;
            AdvancePosition(IsNewLine);
            break;
        }
        default: {
            if (HTNLexerHelpers::IsDigit(Character))
            {
                // Number
                LexNumber(outTokens);
                break;
            }
            else if (HTNLexerHelpers::IsLetter(Character))
            {
                // Identifier
                static const std::unordered_map<std::string, HTNTokenType> Keywords = {{"true", HTNTokenType::TRUE}, {"false", HTNTokenType::FALSE}};
                LexIdentifier(outTokens, Keywords);
                break;
            }

            LOG_HTN_ERROR(mRow, mColumn, "Character [{}] not recognized", HTNLexerHelpers::GetSpecialCharacterEscapeSequence(Character));
            Result = false;
            AdvancePosition();
        }
        }
    }

    AddToken(HTNTokenType::END_OF_FILE, "", HTNAtom(), outTokens);

    return Result;
}
