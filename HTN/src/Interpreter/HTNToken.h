#pragma once

#include "Runtime/HTNAtom.h"

#include <optional>
#include <string>
#include <unordered_map>

enum class HTNTokenType : unsigned int
{
    COLON,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    EXCLAMATION_MARK,
    QUESTION_MARK,
    HASH,

    // Keywords
    HTN_DOMAIN,
    HTN_TOP_LEVEL_DOMAIN,
    HTN_METHOD,
    HTN_TOP_LEVEL_METHOD,
    HTN_AXIOM,
    AND,
    OR,
    ALT,
    NOT,

    IDENTIFIER,
    NUMBER,
    STRING,

    END_OF_FILE
};

inline const std::unordered_map<HTNTokenType, std::string> TokenTypesToStrings = {{HTNTokenType::COLON, "colon"},
                                                                                  {HTNTokenType::LEFT_PARENTHESIS, "left parenthesis"},
                                                                                  {HTNTokenType::RIGHT_PARENTHESIS, "right parenthesis"},
                                                                                  {HTNTokenType::EXCLAMATION_MARK, "exclamation mark"},
                                                                                  {HTNTokenType::QUESTION_MARK, "question mark"},
                                                                                  {HTNTokenType::HASH, "hash"},
                                                                                  {HTNTokenType::HTN_DOMAIN, "domain"},
                                                                                  {HTNTokenType::HTN_TOP_LEVEL_DOMAIN, "top level domain"},
                                                                                  {HTNTokenType::HTN_METHOD, "method"},
                                                                                  {HTNTokenType::HTN_TOP_LEVEL_METHOD, "top level method"},
                                                                                  {HTNTokenType::HTN_AXIOM, "axiom"},
                                                                                  {HTNTokenType::AND, "and"},
                                                                                  {HTNTokenType::OR, "or"},
                                                                                  {HTNTokenType::ALT, "alt"},
                                                                                  {HTNTokenType::NOT, "not"},
                                                                                  {HTNTokenType::IDENTIFIER, "identifier"},
                                                                                  {HTNTokenType::NUMBER, "number"},
                                                                                  {HTNTokenType::STRING, "string"},
                                                                                  {HTNTokenType::END_OF_FILE, "end of file"}};

inline std::string GetTokenTypeString(const HTNTokenType inTokenType)
{
    const auto It = TokenTypesToStrings.find(inTokenType);
    return (It != TokenTypesToStrings.end() ? It->second : "");
}

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
