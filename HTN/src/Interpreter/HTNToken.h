#pragma once

#include <any>
#include <optional>
#include <string>

enum class HTNTokenType : uint8_t
{
	COLON, // :
	LEFT_PARENTHESIS, // (
	RIGHT_PARENTHESIS, // )
	EXCLAMATION_MARK, // !
	QUESTION_MARK, // ?
	HASH, // #

	// Keywords
	HTN_DOMAIN, // domain
	HTN_METHOD, // method
	AND, // and
	NOT, // not

	IDENTIFIER,
	NUMBER,
	STRING,

	END_OF_FILE
};

class HTNToken
{
public:
	explicit HTNToken(const HTNTokenType inType, const std::string& inLexeme, const unsigned int inRow, const unsigned int inColumn, const std::any inValue = std::any());

	HTNTokenType GetType() const;
	const std::string& GetLexeme() const;
	std::any GetValue() const;
	unsigned int GetRow() const;
	unsigned int GetColumn() const;

private:
	HTNTokenType mType;
	std::string mLexeme;
	unsigned int mRow = 0;
	unsigned int mColumn = 0;
	std::optional<std::any> mValue;
};

inline HTNToken::HTNToken(const HTNTokenType inType, const std::string& inLexeme, const unsigned int inRow, const unsigned int inColumn, const std::any inValue)
	: mType(inType), mLexeme(inLexeme), mRow(inRow), mColumn(inColumn), mValue(inValue)
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

inline unsigned int HTNToken::GetRow() const
{
	return mRow;
}

inline unsigned int HTNToken::GetColumn() const
{
	return mColumn;
}
