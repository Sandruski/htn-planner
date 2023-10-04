#pragma once

#include <string>

namespace HTNLexerHelpers
{
bool        IsValidCharacter(const char inCharacter);
bool        IsLetter(const char inCharacter);
bool        IsDigit(const char inCharacter);
bool        IsAlphaNumeric(const char inCharacter);
std::string GetSpecialCharacterEscapeSequence(const char inCharacter);
} // namespace HTNLexerHelpers
