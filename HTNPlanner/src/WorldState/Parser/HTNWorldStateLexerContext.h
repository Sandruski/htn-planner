#pragma once

#include "Parser/HTNLexerContextBase.h"

#include <string>

class HTNWorldStateLexerContext : public HTNLexerContextBase
{
public:
    explicit HTNWorldStateLexerContext(const std::string& inText);
};
