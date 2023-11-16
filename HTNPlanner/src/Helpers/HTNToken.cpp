#include "Helpers/HTNToken.h"

HTNToken::HTNToken(const HTNAtom&            inValue,
                   const HTNTokenType inType HTN_DEBUG_ONLY(, const std::string& inLexeme, const uint32 inRow, const uint32 inColumn))
    : mValue(inValue), mType(inType) HTN_DEBUG_ONLY(, mLexeme(inLexeme), mRow(inRow), mColumn(inColumn))
{
}
