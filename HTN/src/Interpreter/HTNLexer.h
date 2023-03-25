#pragma once

#include <algorithm>
#include <string>
#include <vector>

class HTNToken;

class HTNLexer
{
public:
	explicit HTNLexer(const std::string& inText);

	[[nodiscard]] bool Lex(std::vector<HTNToken>& outTokens);

private:
	[[nodiscard]] bool LexNumber(const char inCharacter, std::vector<HTNToken>& outTokens);

	char GetCharacter(const unsigned int inLookAhead = 0) const;
	void AdvancePosition();

	std::string mText;
	unsigned int mPosition = 0;
};

inline HTNLexer::HTNLexer(const std::string& inText)
	: mText(inText)
{
}

inline char HTNLexer::GetCharacter(const unsigned int inOffset) const
{ 
	const unsigned int Position = mPosition + inOffset;
	return (Position <= mText.length() ? mText[Position] : '\0');
}

inline void HTNLexer::AdvancePosition()
{
	mPosition = std::clamp(static_cast<int>(mPosition) + 1, 0, static_cast<int>(mText.length()));
}
