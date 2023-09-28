#include "WorldState/Parser/HTNWorldStateLexer.h"

#include "Parser/HTNLexerHelpers.h"
#include "HTNToken.h"
#include "HTNAtom.h"
#include "HTNLog.h"

bool HTNWorldStateLexer::Lex(std::vector<HTNToken>& outTokens)
{
    bool Result = true;

    for (char Character = GetCharacter(); HTNLexer::Helpers::IsValidCharacter(Character); Character = GetCharacter())
    {
        switch (Character)
        {
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
            static constexpr bool IsNewLine = true;
            AdvancePosition(IsNewLine);
            break;
        }
        default: {
            if (HTNLexer::Helpers::IsDigit(Character))
            {
                // Number
                LexNumber(outTokens);
                break;
            }
            else if (HTNLexer::Helpers::IsLetter(Character))
            {
                // Identifier
                LexIdentifier(outTokens, {});
                break;
            }

            LOG_HTN_ERROR(mRow, mColumn, "Character [{}] not recognized", HTNLexer::Helpers::GetSpecialCharacterEscapeSequence(Character));
            Result = false;
            AdvancePosition();
        }
        }
    }

    AddToken(HTNTokenType::END_OF_FILE, "", HTNAtom(), outTokens);

    return Result;
}
