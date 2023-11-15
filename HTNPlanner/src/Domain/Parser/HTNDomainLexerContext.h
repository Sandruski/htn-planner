#pragma once

#include "Parser/HTNLexerContextBase.h"

#include <string>

class HTNDomainLexerContext : public HTNLexerContextBase
{
public:
    explicit HTNDomainLexerContext(const std::string& inText);
};
