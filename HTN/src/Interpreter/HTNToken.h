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
	explicit HTNToken(const HTNTokenType inType, const std::string& inLexeme);
	explicit HTNToken(const HTNTokenType inType, const std::string& inLexeme, const std::any inValue);

	HTNTokenType GetType() const;
	std::any GetValue() const;

private:
	HTNTokenType mType;
	std::string mLexeme;
	std::optional<const std::any> mValue;
};

inline HTNToken::HTNToken(const HTNTokenType inType, const std::string& inLexeme)
	: mType(inType), mLexeme(inLexeme)
{
}

inline HTNToken::HTNToken(const HTNTokenType inType, const std::string& inLexeme, const std::any inValue)
	: mType(inType), mLexeme(inLexeme), mValue(inValue)
{
}

inline HTNTokenType HTNToken::GetType() const
{
	return mType;
}

inline std::any HTNToken::GetValue() const
{
	return mValue.value();
}
