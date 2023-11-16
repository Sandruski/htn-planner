#pragma once

#include "HTNCoreMinimal.h"
#include "Parser/HTNLexerBase.h"

class HTNWorldStateLexerContext;

class HTNWorldStateLexer final : public HTNLexerBase
{
public:
    bool Lex(HTNWorldStateLexerContext& ioWorldStateLexerContext) const;
};
