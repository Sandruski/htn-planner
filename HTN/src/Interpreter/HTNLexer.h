#pragma once

#include <any>
#include <string>
#include <vector>

class HTNAtom;
class HTNToken;
enum class HTNTokenType : uint8_t;

// Breaks a text into a series of tokens
// Reports all lexical errors of the text
class HTNLexer
{
public:
	explicit HTNLexer(const std::string& inText);

	bool Lex(std::vector<HTNToken>& outTokens);

private:
	void LexKeywordOrIdentifier(std::vector<HTNToken>& outTokens);
	void LexNumber(std::vector<HTNToken>& outTokens);
	bool LexString(std::vector<HTNToken>& outTokens);
	void LexComment();

	void AddToken(const HTNTokenType inType, const std::string& inLexeme, const HTNAtom& inValue, std::vector<HTNToken>& outTokens) const;

	char GetCharacter(const unsigned int inLookAhead = 0) const;
	void AdvancePosition(const bool inIsNewLine = false);

	std::string mText;
	unsigned int mPosition = 0;
	unsigned int mRow = 0;
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

inline void HTNLexer::AdvancePosition(const bool inIsNewLine)
{
	if (mPosition >= mText.length())
	{
		return;
	}

	++mPosition;

	if (inIsNewLine)
	{
		++mRow;
		mColumn = 0;
	}
	else
	{
		++mColumn;
	}
}
