// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com Jose Antonio Escribano joseantonioescribanoayllon@gmail.com

#include "Parser/HTNParserBase.h"

#include "Parser/HTNParserContextBase.h"
#include "Parser/HTNToken.h"
#include "Parser/HTNTokenHelpers.h"
#include "Parser/HTNTokenType.h"

HTNParserBase::~HTNParserBase() = default;

const HTNToken* HTNParserBase::ParseToken(const HTNTokenType inTokenType, HTNParserContextBase& ioParserContext) const
{
    const uint32    Position = ioParserContext.GetPosition();
    const HTNToken* Token    = ioParserContext.GetToken(Position);
    if (!Token)
    {
#ifdef HTN_ENABLE_LOGGING
        const size             TokensSize       = ioParserContext.GetTokensSize();
        const std::string      LastErrorMessage = std::format("Token at [{}] is out of bounds [{}]", Position, TokensSize);
        static constexpr int32 LastErrorRow     = -1;
        static constexpr int32 LastErrorColumn  = -1;
        ioParserContext.SetLastError(LastErrorMessage, LastErrorRow, LastErrorColumn);
#endif
        return nullptr;
    }

    const HTNTokenType TokenType = Token->GetType();
    if (inTokenType != TokenType)
    {
#ifdef HTN_ENABLE_LOGGING
        const std::string LastErrorMessage =
            std::format("Token [{}] is of type [{}] instead of [{}]", Token->GetLexeme(), HTNTokenHelpers::GetTokenTypeString(Token->GetType()),
                        HTNTokenHelpers::GetTokenTypeString(inTokenType));
        const int32 LastErrorRow    = Token->GetRow();
        const int32 LastErrorColumn = Token->GetColumn();
        ioParserContext.SetLastError(LastErrorMessage, LastErrorRow, LastErrorColumn);
#endif
        return nullptr;
    }

    ioParserContext.AdvancePosition();

    return Token;
}
