// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Helpers/HTNAtom.h"
#include "HTNPlannerMinimal.h"

#include <optional>
#include <string>
#include <unordered_map>

enum class HTNTokenType : uint8
{
    COLON,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    EXCLAMATION_MARK,
    QUESTION_MARK,
    HASH,
    AT,

    // Keywords
    HTN_DOMAIN,
    HTN_TOP_LEVEL_DOMAIN,
    HTN_METHOD,
    HTN_TOP_LEVEL_METHOD,
    HTN_AXIOM,
    HTN_CONSTANTS,
    AND,
    OR,
    ALT,
    NOT,
    TRUE,
    FALSE,

    IDENTIFIER,
    NUMBER,
    STRING,

    END_OF_FILE
};

inline std::string GetTokenTypeString(const HTNTokenType inTokenType)
{
    static const std::unordered_map<HTNTokenType, std::string> TokenTypesToStrings = {{HTNTokenType::COLON, "colon"},
                                                                                      {HTNTokenType::LEFT_PARENTHESIS, "left parenthesis"},
                                                                                      {HTNTokenType::RIGHT_PARENTHESIS, "right parenthesis"},
                                                                                      {HTNTokenType::EXCLAMATION_MARK, "exclamation mark"},
                                                                                      {HTNTokenType::QUESTION_MARK, "question mark"},
                                                                                      {HTNTokenType::HASH, "hash"},
                                                                                      {HTNTokenType::AT, "at"},
                                                                                      {HTNTokenType::HTN_DOMAIN, "domain"},
                                                                                      {HTNTokenType::HTN_TOP_LEVEL_DOMAIN, "top level domain"},
                                                                                      {HTNTokenType::HTN_METHOD, "method"},
                                                                                      {HTNTokenType::HTN_TOP_LEVEL_METHOD, "top level method"},
                                                                                      {HTNTokenType::HTN_AXIOM, "axiom"},
                                                                                      {HTNTokenType::HTN_CONSTANTS, "constants"},
                                                                                      {HTNTokenType::AND, "and"},
                                                                                      {HTNTokenType::OR, "or"},
                                                                                      {HTNTokenType::ALT, "alt"},
                                                                                      {HTNTokenType::NOT, "not"},
                                                                                      {HTNTokenType::IDENTIFIER, "identifier"},
                                                                                      {HTNTokenType::NUMBER, "number"},
                                                                                      {HTNTokenType::STRING, "string"},
                                                                                      {HTNTokenType::END_OF_FILE, "end of file"}};
    const auto                                                 It                  = TokenTypesToStrings.find(inTokenType);
    return (It != TokenTypesToStrings.end() ? It->second : "");
}

class HTNToken
{
public:
    explicit HTNToken(const HTNAtom&            inValue,
                      const HTNTokenType inType HTN_DEBUG_ONLY(, const std::string& inLexeme, const uint32 inRow, const uint32 inColumn));

    const HTNAtom& GetValue() const;
    HTNTokenType   GetType() const;

private:
    HTNAtom      mValue;
    HTNTokenType mType;

#ifdef HTN_DEBUG
public:
    const std::string& GetLexeme() const;
    uint32       GetRow() const;
    uint32       GetColumn() const;

private:
    std::string  mLexeme;
    uint32 mRow    = 0;
    uint32 mColumn = 0;
#endif
};

inline const HTNAtom& HTNToken::GetValue() const
{
    return mValue;
}

inline HTNTokenType HTNToken::GetType() const
{
    return mType;
}

#ifdef HTN_DEBUG
inline const std::string& HTNToken::GetLexeme() const
{
    return mLexeme;
}

inline uint32 HTNToken::GetRow() const
{
    return mRow;
}

inline uint32 HTNToken::GetColumn() const
{
    return mColumn;
}
#endif
