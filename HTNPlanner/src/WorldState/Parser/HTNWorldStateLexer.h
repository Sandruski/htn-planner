#pragma once

#include "Parser/HTNLexerBase.h"

class HTNWorldStateLexer final : public HTNLexerBase
{
public:
    bool Lex(const std::string& inText, std::vector<HTNToken>& outTokens) final;
};
