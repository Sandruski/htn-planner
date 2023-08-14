#pragma once

#include "Runtime/HTNAtom.h"

#include <optional>
#include <string>

enum class HTNTokenType : unsigned int
{
    COLON,             // :
    LEFT_PARENTHESIS,  // (
    RIGHT_PARENTHESIS, // )
    EXCLAMATION_MARK,  // !
    QUESTION_MARK,     // ?
    HASH,              // #

    // Keywords
    HTN_DOMAIN,           // domain
    HTN_TOP_LEVEL_DOMAIN, // top level domain meta
    HTN_METHOD,           // method
    HTN_TOP_LEVEL_METHOD, // top level method meta
    AND,                  // and
    OR,                   // or
    ALT,                  // alt
    NOT,                  // not

    IDENTIFIER,
    NUMBER,
    STRING,

    END_OF_FILE
};

class HTNToken
{
public:
    explicit HTNToken(const HTNTokenType inType, const std::string& inLexeme, const HTNAtom& inValue, const unsigned int inRow,
                      const unsigned int inColumn);

    HTNTokenType       GetType() const;
    const std::string& GetLexeme() const;
    const HTNAtom&     GetValue() const;
    unsigned int       GetRow() const;
    unsigned int       GetColumn() const;

private:
    HTNTokenType mType;
    std::string  mLexeme;
    HTNAtom      mValue;
    unsigned int mRow    = 0;
    unsigned int mColumn = 0;
};

inline HTNToken::HTNToken(const HTNTokenType inType, const std::string& inLexeme, const HTNAtom& inValue, const unsigned int inRow,
                          const unsigned int inColumn)
    : mType(inType), mLexeme(inLexeme), mValue(inValue), mRow(inRow), mColumn(inColumn)
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

inline const HTNAtom& HTNToken::GetValue() const
{
    return mValue;
}

inline unsigned int HTNToken::GetRow() const
{
    return mRow;
}

inline unsigned int HTNToken::GetColumn() const
{
    return mColumn;
}
