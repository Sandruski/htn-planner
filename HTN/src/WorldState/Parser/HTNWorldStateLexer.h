#pragma once

#include "Parser/HTNLexerBase.h"

class HTNWorldStateLexer final : public HTNLexerBase
{
public:
    explicit HTNWorldStateLexer(const std::string& inWorldStateFileText);

    bool Lex(std::vector<HTNToken>& outTokens) final;
};

inline HTNWorldStateLexer::HTNWorldStateLexer(const std::string& inWorldStateFileText) : HTNLexerBase(inWorldStateFileText)
{
}
