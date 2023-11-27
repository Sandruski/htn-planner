// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Domain/Parser/HTNDomainLexer.h"

#include "Domain/Parser/HTNDomainLexerContext.h"
#include "Parser/HTNLexerHelpers.h"
#include "Parser/HTNTokenType.h"

bool HTNDomainLexer::Lex(HTNDomainLexerContext& ioDomainLexerContext) const
{
    OPTICK_EVENT("LexDomain");

    bool Result = true;

    for (char Character = ioDomainLexerContext.GetCharacter(); HTNLexerHelpers::IsValidCharacter(Character);
         Character      = ioDomainLexerContext.GetCharacter())
    {
        switch (Character)
        {
        case ':': {
            // Colon
            ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::COLON HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '(': {
            // Left parenthesis
            ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::LEFT_PARENTHESIS HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case ')': {
            // Right parenthesis
            ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::RIGHT_PARENTHESIS HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '!': {
            // Exclamation mark
            ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::EXCLAMATION_MARK HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '?': {
            // Question mark
            ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::QUESTION_MARK HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '#': {
            // Hash
            ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::HASH HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '@': {
            // At
            ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::AT HTN_DEBUG_ONLY(, std::string(1, Character)));
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '/': {
            static constexpr uint32 LookAhead     = 1;
            const char              NextCharacter = ioDomainLexerContext.GetCharacter(LookAhead);
            if (NextCharacter == '/')
            {
                // Comment
                LexComment(ioDomainLexerContext);
                break;
            }

#ifdef HTN_DEBUG
            const std::string Message = std::format("Expected '/' after [{}] for a comment", Character);
            HTNLexerHelpers::PrintError(Message, ioDomainLexerContext);
#endif
            Result = false;
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '"': {
            // String
            Result = LexString(ioDomainLexerContext) && Result;
            break;
        }
        case ' ': {
            // Whitespace
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '\t': {
            // Tab
            ioDomainLexerContext.AdvancePosition();
            break;
        }
        case '\n': {
// Newline
#ifdef HTN_DEBUG
            static constexpr bool IsNewLine = true;
#endif
            ioDomainLexerContext.AdvancePosition(HTN_DEBUG_ONLY(IsNewLine));
            break;
        }
        default: {
            if (HTNLexerHelpers::IsDigit(Character))
            {
                // Number
                LexNumber(ioDomainLexerContext);
                break;
            }
            else if (HTNLexerHelpers::IsLetter(Character))
            {
                // Identifier
                static const std::unordered_map<std::string, HTNTokenType> Keywords = {
                    {"domain", HTNTokenType::HTN_DOMAIN}, {"top_level_domain", HTNTokenType::HTN_TOP_LEVEL_DOMAIN},
                    {"method", HTNTokenType::HTN_METHOD}, {"top_level_method", HTNTokenType::HTN_TOP_LEVEL_METHOD},
                    {"axiom", HTNTokenType::HTN_AXIOM},   {"constants", HTNTokenType::HTN_CONSTANTS},
                    {"and", HTNTokenType::AND},           {"or", HTNTokenType::OR},
                    {"alt", HTNTokenType::ALT},           {"not", HTNTokenType::NOT},
                    {"true", HTNTokenType::TRUE},         {"false", HTNTokenType::FALSE}};
                LexIdentifier(Keywords, ioDomainLexerContext);
                break;
            }

#ifdef HTN_DEBUG
            const std::string Message = std::format("Character [{}] not recognized", HTNLexerHelpers::GetSpecialCharacterEscapeSequence(Character));
            HTNLexerHelpers::PrintError(Message, ioDomainLexerContext);
#endif
            Result = false;
            ioDomainLexerContext.AdvancePosition();
        }
        }
    }

    ioDomainLexerContext.AddToken(HTNAtom(), HTNTokenType::END_OF_FILE HTN_DEBUG_ONLY(, ""));

    return Result;
}
