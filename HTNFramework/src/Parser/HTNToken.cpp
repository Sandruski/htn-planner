// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Parser/HTNToken.h"

HTNToken::HTNToken(const HTNAtom&            inValue,
                   const HTNTokenType inType HTN_LOG_ONLY(, const std::string& inLexeme, const uint32 inRow, const uint32 inColumn))
    : mValue(inValue), mType(inType) HTN_LOG_ONLY(, mLexeme(inLexeme), mRow(inRow), mColumn(inColumn))
{
}
