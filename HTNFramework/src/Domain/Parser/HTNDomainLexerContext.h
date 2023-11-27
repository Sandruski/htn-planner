// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNLexerContextBase.h"

#include <string>
#include <vector>

class HTNToken;

/**
 * Data of a lexer of a domain
 */
class HTNDomainLexerContext final : public HTNLexerContextBase
{
public:
    explicit HTNDomainLexerContext(const std::string& inText, std::vector<HTNToken>& outTokens);
};
