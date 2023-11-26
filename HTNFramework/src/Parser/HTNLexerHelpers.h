// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"

#include <string>

#ifdef HTN_DEBUG
class HTNLexerContextBase;
#endif

namespace HTNLexerHelpers
{
HTN_NODISCARD bool IsValidCharacter(const char inCharacter);
HTN_NODISCARD bool IsLetter(const char inCharacter);
HTN_NODISCARD bool IsDigit(const char inCharacter);
HTN_NODISCARD bool IsAlphaNumeric(const char inCharacter);
HTN_NODISCARD std::string GetSpecialCharacterEscapeSequence(const char inCharacter);

#ifdef HTN_DEBUG
void PrintErrorMessage(const std::string& inMessage, const HTNLexerContextBase& inLexerContext);
#endif
} // namespace HTNLexerHelpers
