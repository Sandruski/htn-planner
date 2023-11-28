// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

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
    // Main lex member method
    bool Lex(HTNDomainLexerContext& ioDomainLexerContext) const;
};
