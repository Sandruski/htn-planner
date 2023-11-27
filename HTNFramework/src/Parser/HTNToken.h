// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#pragma once

#include "Core/HTNAtom.h"
#include "HTNCoreMinimal.h"
#include "Parser/HTNTokenType.h"

#include <string>

class HTNToken
{
public:
    explicit HTNToken(const HTNAtom&            inValue,
                      const HTNTokenType inType HTN_DEBUG_ONLY(, const std::string& inLexeme, const uint32 inRow, const uint32 inColumn));

    HTN_NODISCARD const HTNAtom& GetValue() const;
    HTN_NODISCARD HTNTokenType   GetType() const;

private:
    HTNAtom      mValue;
    HTNTokenType mType;

#ifdef HTN_DEBUG
public:
    HTN_NODISCARD const std::string& GetLexeme() const;
    HTN_NODISCARD uint32             GetRow() const;
    HTN_NODISCARD uint32             GetColumn() const;

private:
    std::string mLexeme;
    uint32      mRow    = 0;
    uint32      mColumn = 0;
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
