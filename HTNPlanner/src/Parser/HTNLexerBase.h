// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"

#include <string>
#include <unordered_map>

class HTNLexerContextBase;
enum class HTNTokenType : uint8;

/**
 * Breaks a text into a series of tokens
 * Reports all lexical errors of the text
 */
class HTNLexerBase
{
public:
    virtual ~HTNLexerBase() = 0;

protected:
    void LexIdentifier(const std::unordered_map<std::string, HTNTokenType>& inKeywords, HTNLexerContextBase& ioLexerContext) const;
    void LexNumber(HTNLexerContextBase& ioLexerContext) const;
    bool LexString(HTNLexerContextBase& ioLexerContext) const;
    void LexComment(HTNLexerContextBase& ioLexerContext) const;
};
