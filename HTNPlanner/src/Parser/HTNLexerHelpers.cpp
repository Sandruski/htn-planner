#include "Parser/HTNLexerHelpers.h"

#ifdef HTN_DEBUG
#include "Parser/HTNLexerContextBase.h"
#endif

namespace HTNLexerHelpers
{
bool IsValidCharacter(const char inCharacter)
{
    return (inCharacter != '\0');
}

bool IsLetter(const char inCharacter)
{
    return (((inCharacter >= 'a') && (inCharacter <= 'z')) || ((inCharacter >= 'A') && (inCharacter <= 'Z')) ||
            (inCharacter >= '_')); // Underscore is considered a letter
}

bool IsDigit(const char inCharacter)
{
    return ((inCharacter >= '0') && (inCharacter <= '9'));
}

bool IsAlphaNumeric(const char inCharacter)
{
    return (IsLetter(inCharacter) || IsDigit(inCharacter));
}

std::string GetSpecialCharacterEscapeSequence(const char inCharacter)
{
    static const std::unordered_map<char, std::string> SpecialCharactersToEscapeSequences = {{'\t', "\\t"}, {'\n', "\\n"}};

    const auto It = SpecialCharactersToEscapeSequences.find(inCharacter);
    return ((It != SpecialCharactersToEscapeSequences.end()) ? It->second : std::string(1, inCharacter));
}

#ifdef HTN_DEBUG
void PrintErrorMessage(const std::string& inMessage, HTNLexerContextBase& ioLexerContext)
{
    const uint32 Row    = ioLexerContext.GetRow();
    const uint32 Column = ioLexerContext.GetColumn();
    HTN_DOMAIN_LOG_ERROR(Row, Column, "{}", inMessage);
}
#endif
} // namespace HTNLexerHelpers
