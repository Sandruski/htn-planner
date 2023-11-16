#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNLexerContextBase.h"

#include <string>
#include <vector>

class HTNToken;

class HTNWorldStateLexerContext : public HTNLexerContextBase
{
public:
    explicit HTNWorldStateLexerContext(const std::string& inText, std::vector<HTNToken>& outTokens);
};
