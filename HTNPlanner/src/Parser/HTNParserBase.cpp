// Copyright (c) 2023 Sandra Alvarez sandruskiag@gmail.com

#include "Parser/HTNParserBase.h"

#include "Helpers/HTNToken.h"
#include "Parser/HTNParserContextBase.h"

HTNParserBase::~HTNParserBase() = default;

const HTNToken* HTNParserBase::ParseToken(const HTNTokenType inTokenType, HTNParserContextBase& ioParserContext) const
{
    const uint32 Position = ioParserContext.GetPosition();
    const HTNToken*    Token    = ioParserContext.GetToken(Position);
    if (!Token)
    {
#if HTN_DEBUG
        const size TokensSize       = ioParserContext.GetTokensSize();
        const std::string LastErrorMessage = std::format("Token at [{}] is out of bounds [{}]", Position, TokensSize);
        ioParserContext.SetLastErrorMessage(LastErrorMessage);
        static constexpr int32 LastErrorRow = -1;
        ioParserContext.SetLastErrorRow(LastErrorRow);
        static constexpr int32 LastErrorColumn = -1;
        ioParserContext.SetLastErrorColumn(LastErrorColumn);
#endif
        return nullptr;
    }

    const HTNTokenType TokenType = Token->GetType();
    if (inTokenType != TokenType)
    {
#if HTN_DEBUG
        const std::string LastErrorMessage = std::format("Token [{}] is of type [{}] instead of [{}]", Token->GetLexeme(),
                                                         GetTokenTypeString(Token->GetType()), GetTokenTypeString(inTokenType));
        ioParserContext.SetLastErrorMessage(LastErrorMessage);
        const int32 LastErrorRow = Token->GetRow();
        ioParserContext.SetLastErrorRow(LastErrorRow);
        const int32 LastErrorColumn = Token->GetColumn();
        ioParserContext.SetLastErrorColumn(LastErrorColumn);
#endif
        return nullptr;
    }

    ioParserContext.IncrementPosition();

    return Token;
}
