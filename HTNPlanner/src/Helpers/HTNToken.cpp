#include "Helpers/HTNToken.h"

HTNToken::HTNToken(const HTNTokenType inType, const std::string& inLexeme, const HTNAtom& inValue, const unsigned int inRow,
                   const unsigned int inColumn)
    : mType(inType), mLexeme(inLexeme), mValue(inValue), mRow(inRow), mColumn(inColumn)
{
}
