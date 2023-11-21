// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"

#include <string>

#ifdef HTN_DEBUG
class HTNLexerContextBase;
#endif

namespace HTNLexerHelpers
{
bool        IsValidCharacter(const char inCharacter);
bool        IsLetter(const char inCharacter);
bool        IsDigit(const char inCharacter);
bool        IsAlphaNumeric(const char inCharacter);
std::string GetSpecialCharacterEscapeSequence(const char inCharacter);

#ifdef HTN_DEBUG
void PrintErrorMessage(const std::string& inMessage, HTNLexerContextBase& ioLexerContext);
#endif
} // namespace HTNLexerHelpers
