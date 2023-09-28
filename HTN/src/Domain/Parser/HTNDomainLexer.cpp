#include "Domain/Parser/HTNDomainLexer.h"

#include "HTNAtom.h"
#include "Parser/HTNLexerHelpers.h"
#include "HTNToken.h"
#include "HTNLog.h"

bool HTNDomainLexer::Lex(std::vector<HTNToken>& outTokens)
{
    bool Result = true;

    for (char Character = GetCharacter(); HTNLexer::Helpers::IsValidCharacter(Character); Character = GetCharacter())
    {
        switch (Character)
        {
        case ':': {
            // Colon
            AddToken(HTNTokenType::COLON, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
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
        case '!': {
            // Exclamation mark
            AddToken(HTNTokenType::EXCLAMATION_MARK, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '?': {
            // Question mark
            AddToken(HTNTokenType::QUESTION_MARK, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '#': {
            // Hash
            AddToken(HTNTokenType::HASH, std::string(1, Character), HTNAtom(), outTokens);
            AdvancePosition();
            break;
        }
        case '@': {
            // At
            AddToken(HTNTokenType::AT, std::string(1, Character), HTNAtom(), outTokens);
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
        case '\t': {
            // Tab
            AdvancePosition();
            break;
        }
        case '\n': {
            // Newline      
            static constexpr bool IsNewLine = true;
            AdvancePosition(true);
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
                static const std::unordered_map<std::string, HTNTokenType> Keywords = {
                    {"domain", HTNTokenType::HTN_DOMAIN}, {"top_level_domain", HTNTokenType::HTN_TOP_LEVEL_DOMAIN},
                    {"method", HTNTokenType::HTN_METHOD}, {"top_level_method", HTNTokenType::HTN_TOP_LEVEL_METHOD},
                    {"axiom", HTNTokenType::HTN_AXIOM},   {"constants", HTNTokenType::HTN_CONSTANTS},
                    {"and", HTNTokenType::AND},           {"or", HTNTokenType::OR},
                    {"alt", HTNTokenType::ALT},           {"not", HTNTokenType::NOT}};
                LexIdentifier(outTokens, Keywords);
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
