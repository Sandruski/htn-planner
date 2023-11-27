// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNLexerBase.h"

class HTNWorldStateLexerContext;

/**
 * Breaks a world state text into a series of tokens
 */
class HTNWorldStateLexer final : public HTNLexerBase
{
public:
    bool Lex(HTNWorldStateLexerContext& ioWorldStateLexerContext) const;
};
