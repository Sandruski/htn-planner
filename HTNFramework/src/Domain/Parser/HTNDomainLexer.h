// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNLexerBase.h"

class HTNDomainLexerContext;

/**
 * Breaks a domain text into a series of tokens
 */
class HTNDomainLexer final : public HTNLexerBase
{
public:
    bool Lex(HTNDomainLexerContext& ioDomainLexerContext) const;
};
