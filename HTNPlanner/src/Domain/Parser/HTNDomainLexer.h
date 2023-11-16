#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNLexerBase.h"

class HTNDomainLexerContext;

class HTNDomainLexer final: public HTNLexerBase
{
public:
    bool Lex(HTNDomainLexerContext& ioDomainLexerContext) const;
};
