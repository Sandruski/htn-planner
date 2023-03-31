#pragma once

#include <any>
#include <string>
#include <vector>

class HTNToken;
enum class HTNTokenType : uint8_t;

// Breaks a text into a series of tokens
// Reports all lexical errors of the text
class HTNLexer
{
public:
	explicit HTNLexer(const std::string& inText);

	[[nodiscard]] bool Lex(std::vector<HTNToken>& outTokens);

private:
	[[nodiscard]] bool LexNumber(const char inCharacter, std::vector<HTNToken>& outTokens);

	void AddToken(const HTNTokenType inType, const std::string& inLexeme, std::vector<HTNToken>& outTokens, const std::any inValue = std::any()) const;

	char GetCharacter(const unsigned int inLookAhead = 0) const;
	void AdvancePosition();

	// Text to lex
	std::string mText;

	// Current position in the text
	unsigned int mPosition = 0;

	// Current line in the text
	unsigned int mLine = 0;

	// Current column in the text
	unsigned int mColumn = 0;
};

inline HTNLexer::HTNLexer(const std::string& inText)
	: mText(inText)
{
}

inline char HTNLexer::GetCharacter(const unsigned int inOffset) const
{
	const unsigned int Position = mPosition + inOffset;
	if (Position <= mText.length())
	{
		return mText[Position];
	}

	return '\0';
}

inline void HTNLexer::AdvancePosition()
{
	if (mPosition >= mText.length())
	{
		return;
	}

	++mPosition;
	++mColumn;
}
