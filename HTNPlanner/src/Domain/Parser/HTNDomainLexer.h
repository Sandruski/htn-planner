#pragma once

#include "Parser/HTNLexerBase.h"

#include <string>

class HTNDomainLexer final: public HTNLexerBase
{
public:
	bool Lex(const std::string& inText, std::vector<HTNToken>& outTokens) final;
};
