#pragma once

#include "Parser/HTNLexerBase.h"

#include <string>

class HTNDomainLexer final: public HTNLexerBase
{
public:
    explicit HTNDomainLexer(const std::string& inDomainFileText);

	bool Lex(std::vector<HTNToken>& outTokens) final;
};

inline HTNDomainLexer::HTNDomainLexer(const std::string& inDomainFileText)
	: HTNLexerBase(inDomainFileText)
{
}
