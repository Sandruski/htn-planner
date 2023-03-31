#pragma once

#include <any>
#include <optional>
#include <string>

enum class HTNTokenType : uint8_t
{
	PLUS, // Addition
	MINUS, // Subtraction
	STAR, // Multiplication
	SLASH, // Division

	LEFT_PAREN, // Left parenthesis
	RIGHT_PAREN, // Right parenthesis

	NUMBER, // Double

	END_OF_FILE
};

class HTNToken
{
public:
	explicit HTNToken(const HTNTokenType inType, const std::string& inLexeme, const unsigned int inLine, const unsigned int inColumn, const std::any inValue = std::any());

	HTNTokenType GetType() const;
	const std::string& GetLexeme() const;
	std::any GetValue() const;
	unsigned int GetLine() const;
	unsigned int GetColumn() const;

private:
	HTNTokenType mType;
	std::string mLexeme;
	std::optional<std::any> mValue;
	unsigned int mLine = 0;
	unsigned int mColumn = 0;
};

inline HTNToken::HTNToken(const HTNTokenType inType, const std::string& inLexeme, const unsigned int inLine, const unsigned int inColumn, const std::any inValue)
	: mType(inType), mLexeme(inLexeme), mLine(inLine), mColumn(inColumn), mValue(inValue)
{
}

inline HTNTokenType HTNToken::GetType() const
{
	return mType;
}

inline const std::string& HTNToken::GetLexeme() const
{
	return mLexeme;
}

inline std::any HTNToken::GetValue() const
{
	return mValue.value();
}

inline unsigned int HTNToken::GetLine() const
{
	return mLine;
}

inline unsigned int HTNToken::GetColumn() const
{
	return mColumn;
}
