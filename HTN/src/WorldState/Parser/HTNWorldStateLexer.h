#pragma once

#include "HTNLexerBase.h"

class HTNWorldStateLexer final : public HTNLexerBase
{
public:
    explicit HTNWorldStateLexer(const std::string& inWorldStateFileText);
};

inline HTNWorldStateLexer::HTNWorldStateLexer(const std::string& inWorldStateFileText) : HTNLexerBase(inWorldStateFileText)
{
}
