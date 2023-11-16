#pragma once

class HTNParserContextBase;
class HTNToken;
enum class HTNTokenType : unsigned char;

class HTNParserBase
{
public:
    virtual ~HTNParserBase() = 0;

protected:
    const HTNToken* ParseToken(const HTNTokenType inTokenType, HTNParserContextBase& ioParserContext) const;
};
