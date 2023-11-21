// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNPlannerMinimal.h"
#include "Parser/HTNLexerBase.h"

class HTNDomainLexerContext;

class HTNDomainLexer final: public HTNLexerBase
{
public:
    bool Lex(HTNDomainLexerContext& ioDomainLexerContext) const;
};
