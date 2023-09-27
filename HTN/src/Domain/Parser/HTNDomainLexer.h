#pragma once

#include "HTNLexerBase.h"

class HTNDomainLexer final: public HTNLexerBase
{
public:
    explicit HTNDomainLexer(const std::string& inDomainFileText);
};

inline HTNDomainLexer::HTNDomainLexer(const std::string& inDomainFileText)
	: HTNLexerBase(inDomainFileText)
{
}
